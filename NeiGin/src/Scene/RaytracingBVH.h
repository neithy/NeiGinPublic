#pragma once

#include "NeiGinBase.h"
#include "NeiVu/DeviceObject.h"

#include "Material.h"
#include "NeiVu/AccelerationStructure.h"

namespace Nei{
  struct RaytracingInstanceData {
    Ptr<Buffer> vertexBuffer;
    uint vertexBufferOffset=0;
    Ptr<Buffer> indexBuffer;
    uint indexBufferOffset=0;
    Ptr<Material> material;
  };

  class NEIGIN_EXPORT RaytracingBVH : public DeviceObject{
  public:
    RaytracingBVH(DeviceContext* dc);
    virtual ~RaytracingBVH();

    void setUpdatable(bool top, bool bottom){
      updatableTop=top;
      updatableBottom=bottom;}
    
    void buildTop(CommandBuffer* cmd);
    void buildBottom(CommandBuffer* cmd, Mesh* mesh);

    void updateTop(CommandBuffer* cmd);
    void updateBottom(CommandBuffer* cmd);

    AccelerationStructure* getTop() const;

    void compactBottom();
  protected:
    bool updatableTop = false;
    bool updatableBottom = false;
    std::vector<vk::GeometryInstance> instances;
    std::vector<vk::GeometryNV> geometries;

    Ptr<AccelerationStructure> topLevel;
    Ptr<AccelerationStructure> bottomLevel;
  };
};


