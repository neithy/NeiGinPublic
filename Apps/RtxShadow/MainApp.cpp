#include "MainApp.h"
#include "Scene/RaytracingBVH.h"

#define fly 1
#define rtx 1

MainApp::MainApp(int argc, char** argv): SimpleApplication(nullptr) {
  args.init(argc, argv);
  resolution = {args.w * args.renderScale, args.h * args.renderScale};

  Options opt;
  opt.vsync = false;

#if rtx
  opt.deviceExtensions.push_back(VK_NV_RAY_TRACING_EXTENSION_NAME);
#endif

#ifdef DEBUG
  opt.validation = true;
#else
  opt.validation = false;
#endif

  opt.window.borderless = true;
  opt.window.resizable = false;
  opt.window.size = {args.w, args.h};
  init(opt);

  NeiFS->mount(".");
  NeiFS->mount(APP_DIR);  

  auto& dc = deviceContext;
  Ptr cmd = new CommandBuffer(dc);


  profiler = new Profiler(dc);
  if(!args.log.empty()) {
    profiler->openLog(args.log);
  }

  profiler->init(6, args.avgFrames, args.avgFrames * args.frames);

  // Model
  model = Loader::load(dc, NeiFS->resolve(args.model));
  if(!args.flythrough.empty()) {
    cameraPath = CameraPath(args.frames==0, NeiFS->resolve(args.flythrough).string());
  }

  lightPosition = args.light;

#if rtx
  {
    auto start = std::chrono::high_resolution_clock::now();
    cmd->begin();
    bvh = new RaytracingBVH(dc);
    bvh->setUpdatable(args.bvh>=1,args.bvh>=2);
    bvh->buildBottom(cmd, model.mesh);

    if(args.bvh == 0) { // compact if static bvh
      cmd->end();
      cmd->submit();
      bvh->compactBottom();
      cmd->begin();
    }

    bvh->buildTop(cmd);
    cmd->end();
    cmd->submit();
    auto end = std::chrono::high_resolution_clock::now();

    nei_log("BVH build time {} ms", std::chrono::duration<double, std::milli>(end-start).count());
  }
#endif


  // Pipelines 
  gbufferPipeline = dc->loadFx(NeiFS->resolve("shaders/gbuffer.fx"));
  gbufferPipeline->addVertexLayout(VertexLayout::defaultLayout());
#if rtx
  shadowMaskPipeline = dc->getFxLoader()->loadFxFile(NeiFS->resolve("shaders/shadowmask.fx")).as<RaytracingPipeline>();
  sbt = shadowMaskPipeline->createShaderBindingTable();
#endif
  lightingPipeline = dc->loadComp(NeiFS->resolve("shaders/lighting.fx"));


  // Gbuffer
  gbuffer = new GBuffer(dc);
  gbuffer->addColorLayer(vk::Format::eR32G32B32A32Sfloat, "gbufferPosition", vk::ImageLayout::eGeneral);
  gbuffer->addColorLayer(vk::Format::eR16G16B16A16Sfloat, "gbufferNormal", vk::ImageLayout::eGeneral);
  gbuffer->addColorLayer(vk::Format::eR8G8B8A8Unorm, "gbufferColor", vk::ImageLayout::eGeneral);
  gbuffer->addDepthLayer();
  gbuffer->resize(resolution);

  accBuffer = new Texture2D(dc, resolution, vk::Format::eR8G8B8A8Unorm, Texture::Usage::GBuffer, false);

  // Shadow Mask
  shadowMask = new Texture2D(dc, resolution, vk::Format::eR8Unorm, Texture::Usage::GBuffer, false);

  cmd->begin();
  shadowMask->setLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, shadowMask->getFullRange());
  cmd->end();
  cmd->submit();

  //Descriptors
  gbufferDescriptor = gbufferPipeline->allocateDescriptorSet();
  std::vector<vk::ImageView> views;
  for(auto& t : model.textures) views.push_back(t->createView());
  while(views.size() < 128) views.push_back(views[0]);
  gbufferDescriptor->update(0, views, dc->getSampler(SamplerType::linearRepeat));


  lightingDescriptor = lightingPipeline->allocateDescriptorSet();
  lightingDescriptor->update(0, accBuffer->createView());
  lightingDescriptor->update(1, gbuffer->getLayer(0)->createView());
  lightingDescriptor->update(2, gbuffer->getLayer(1)->createView());
  lightingDescriptor->update(3, gbuffer->getLayer(2)->createView());
  lightingDescriptor->update(4, shadowMask->createView());

#if rtx
  shadowMaskDescriptor = shadowMaskPipeline->allocateDescriptorSet();
  shadowMaskDescriptor->update(0, shadowMask->createView());
  shadowMaskDescriptor->update(1, bvh->getTop());
  shadowMaskDescriptor->update(2, gbuffer->getLayer(0)->createView());
#endif

  commandBuffers[0] = new CommandBuffer(dc);
  commandBuffers[1] = new CommandBuffer(dc);
  commandBuffers[2] = new CommandBuffer(dc);
  commandBuffers[3] = new CommandBuffer(dc);
}

void MainApp::update(Nei::AppFrame const& frame) {
  profiler->checkResults();

  if(args.frames > 0 && (frame.frameId - skipFrames) > args.frames * args.avgFrames) {
    profiler->finish();
    quit();
  }
}

void MainApp::draw() {
  if(window->isClosed()) return;
  if(!swapchain->isValid()) return;

  auto& cmd = commandBuffers[currentFrame];
  currentFrame = (currentFrame + 1) % 4;
  cmd->wait();

  Scope frameScope(swapchain);
  {
    Scope commandScope(cmd);

    profiler->beginFrame(cmd, frame.frameId - skipFrames);

    profiler->writeMarker(cmd);
    {
      ProfileGPU(cmd, "BVH update");
      
      if(args.bvh==1)
        bvh->updateTop(cmd);
      if(args.bvh==2) {
        bvh->updateBottom(cmd);
        bvh->updateTop(cmd);
      }
      cmd->debugBarrier();      
    }

    profiler->writeMarker(cmd);

    {
      ProfileGPU(cmd, "GBuffer");

      {
        Scope renderPass(gbuffer, cmd);
        cmd->bind(gbufferPipeline);

        mat4 vp;
#ifdef fly
        if(!args.flythrough.empty()) {
            if (args.frames > 0) {
                CameraPathKeypoint const kp = cameraPath.getKeypoint(max<float>(0, (frame.frameId - skipFrames)/float(args.frames) / float(args.avgFrames)));
                glm::mat4 const viewMat = glm::lookAt(kp.position, kp.position + kp.viewVector, kp.upVector);
                vp = camera->getProjection() * viewMat;
            }
            else{
              CameraPathKeypoint const kp = cameraPath.getKeypoint(float(frame.simTime/60.));
              glm::mat4 const viewMat = glm::lookAt(kp.position, kp.position + kp.viewVector, kp.upVector);
              vp = camera->getProjection() * viewMat;
            }
        } else {
          vp = camera->getProjection() * camera->getView();
        }
#else
        vp = camera->getProjection() * camera->getView();
#endif

        gbufferPipeline->setConstants(cmd, vp, 0, vk::ShaderStageFlagBits::eVertex);

        cmd->bind(gbufferDescriptor);
        model.mesh->draw(cmd);
      }
    }

    profiler->writeMarker(cmd);

#if rtx
    {
      ProfileGPU(cmd, "ShadowMask");
      cmd->bind(shadowMaskPipeline);
      cmd->bind(shadowMaskDescriptor);
      shadowMaskPipeline->setConstants(cmd, lightPosition, 0, vk::ShaderStageFlagBits::eRaygenNV);
      cmd->raytrace(sbt, ivec3(resolution, 1));
      cmd->debugBarrier();
    }
#endif

    profiler->writeMarker(cmd);

    {
      ProfileGPU(cmd, "Lighting");
      accBuffer->setLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, accBuffer->getFullRange());
      cmd->bind(lightingPipeline);
      lightingPipeline->setConstants(cmd, lightPosition, 0, vk::ShaderStageFlagBits::eCompute);
      lightingPipeline->setConstants(cmd, manipulator->getEye(), sizeof(vec4), vk::ShaderStageFlagBits::eCompute);

      cmd->bind(lightingDescriptor);
      cmd->dispatch(uvec3((resolution.x + 7) / 8, (resolution.y + 7) / 8, 1));
      accBuffer->setLayout(cmd, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal,
        accBuffer->getFullRange());
      cmd->debugBarrier();
    }

    profiler->writeMarker(cmd);

    swapchain->copy(cmd, accBuffer);

    profiler->writeMarker(cmd);
    ProfileCollect(cmd);
  }
  cmd->submit(swapchain);
}
