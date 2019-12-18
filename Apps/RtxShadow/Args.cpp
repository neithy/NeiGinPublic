#include "Args.h"
#include <iostream>

const char* helpString =
  R".(
Usage:
-w 1920 - width
-h 1080 - height
-p "" - preset 0=sponza, 1=conference, 2=Closed Citadel, 3=Budha, 4=Hairball 
-m models/sponza/sponza.obj - model path
-f models/sponza/sponza_fly.txt - fly path
-s 0 1000 0 1 - sun position
-t 0 - total frames to record (overrides speed)
-l "" - log to file - "frameID,bvh,gbuffer,shadowmask,lighting,copy"
-r 1 - render scale (allows to 4k on 1080p display)
-a 1 - average render times from N frames
-b 0 - Build BVH 0=once, 1=update top every frame, 2=update top+bottom every frame, 3=full rebuild
).";


void Args::init(int argc, char** argv) {
  std::string arg;
  auto next = [&]() {
    arg = *argv;
    argv++;
    argc--;
  };
  next();

  while(argc) {
    next();

    if(arg == "-w" && argc) {
      next();
      w = std::stoi(arg);
    } else if(arg == "-h" && argc) {
      next();
      h = std::stoi(arg);
    } else if(arg == "-m" && argc) {
      next();
      model = arg;
    } else if(arg == "-f" && argc) {
      next();
      flythrough = arg;
    } else if(arg == "-s" && argc>=4) {
      next();
      light.x = std::stof(arg);
      next();
      light.y = std::stof(arg);
      next();
      light.z = std::stof(arg);
      next();
    } else if(arg == "-t" && argc) {
      next();
      frames = std::stoi(arg);
    } else if(arg == "-l" && argc) {
      next();
      log = arg;
    } else if(arg == "-r" && argc) {
      next();
      renderScale = std::stof(arg);
    } else if(arg == "-a" && argc) {
      next();
      avgFrames = std::stoi(arg);
    } else if(arg == "-b" && argc) {
      next();
      bvh = std::stoi(arg);
    } else if(arg == "-b" && argc) {
      next();
      bvh = std::stoi(arg);
    } else if(arg == "-p" && argc) {
      next();
      int preset = std::stoi(arg);
      switch(preset) {
        case 0:
          model = "models/sponza/sponza.obj";
          flythrough = "models/sponza/sponza_fly.txt";
          light = glm::vec3(0,1000,0);
          break;
        case 1:
          model = "models/conference/conference.obj";
          flythrough = "models/conference/conference_fly.txt";
          light = glm::vec3(198,620,-182.5);
          break;
        case 2:
          model = "models/citadel/mycitadel.obj";
          flythrough = "models/citadel/citadel_fly.txt";
          light = glm::vec3(-1658, 1877, 1031);
          break;
        case 3:
          model = "models/buddha/buddha_plane.obj";
          flythrough = "models/buddha/buddha_fly.txt";
          light = glm::vec3(0 ,2, 1.5 );
          break;
        case 4:
          model = "models/hairball/hairball_plane.obj";
          flythrough = "models/hairball/hairball_fly.txt";
          light = glm::vec3(0,10,0);
          break;
      }
    } else {
      std::cout << helpString;
      exit(0);
    }
  }
}