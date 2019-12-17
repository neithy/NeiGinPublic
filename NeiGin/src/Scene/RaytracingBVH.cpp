#include "RaytracingBVH.h"
#include "NeiVu/AccelerationStructure.h"
#include "Mesh.h"
#include "NeiVu/Buffer.h"
#include "Assets/MeshBuffer.h"

using namespace Nei;

RaytracingBVH::RaytracingBVH(DeviceContext* dc): DeviceObject(dc) { }

RaytracingBVH::~RaytracingBVH() { }

void RaytracingBVH::buildTop(CommandBuffer* cmd) {
  mat4 tr(1);
  vk::GeometryInstance instance;
  memcpy(instance.transform, &tr, sizeof(instance.transform));
  instance.instanceId = instances.size();
  instance.mask = 0xFF;
  instance.instanceOffset = 0;
  instance.flags = uint(vk::GeometryInstanceFlagBitsNV::eTriangleCullDisable);
  instance.accelerationStructureHandle = bottomLevel->getHandle();
  instances.push_back(instance);

  topLevel = new AccelerationStructure(deviceContext);
  topLevel->create(cmd, instances);
}

void RaytracingBVH::buildBottom(CommandBuffer* cmd, Mesh* mesh) {
  vk::GeometryNV geometry;
  geometry.geometryType = vk::GeometryTypeNV::eTriangles;
  geometry.geometry.triangles.vertexData = *mesh->getVertexBuffer();
  geometry.geometry.triangles.vertexOffset = mesh->getVertexBufferOffset();
  geometry.geometry.triangles.vertexCount = mesh->getVertexCount();
  geometry.geometry.triangles.vertexStride = mesh->getVertexLayout().stride;
  assert(mesh->getVertexLayout().attributes[0].format == vk::Format::eR32G32B32Sfloat);
  geometry.geometry.triangles.vertexFormat = vk::Format::eR32G32B32Sfloat;
  geometry.geometry.triangles.indexData = *mesh->getIndexBuffer();
  geometry.geometry.triangles.indexOffset = mesh->getIndexBufferOffset();
  geometry.geometry.triangles.indexCount = mesh->getIndexCount();
  geometry.geometry.triangles.indexType = vk::IndexType::eUint32;
  geometry.geometry.triangles.transformData = nullptr;
  geometry.geometry.triangles.transformOffset = 0;
  geometry.flags = vk::GeometryFlagBitsNV::eOpaque;
  geometries.push_back(geometry);

  bottomLevel = new AccelerationStructure(deviceContext);
  bottomLevel->create(cmd, geometries);
}


AccelerationStructure* RaytracingBVH::getTop() const {
  return topLevel;
}

void RaytracingBVH::compactBottom() {
  bottomLevel->compact();
}
