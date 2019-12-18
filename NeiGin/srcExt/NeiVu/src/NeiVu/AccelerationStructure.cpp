#include "AccelerationStructure.h"

#include "Buffer.h"
#include "CommandBuffer.h"
#include "MemoryManager.h"

using namespace Nei::Vu;
AccelerationStructure::AccelerationStructure(DeviceContext* dc): DeviceObject(dc) { }

AccelerationStructure::~AccelerationStructure() {
  auto device = deviceContext->getVkDevice();
  auto dispatch = deviceContext->getDispatch();
  if(structure) device.destroyAccelerationStructureNV(structure, nullptr, dispatch);
}

void AccelerationStructure::build(CommandBuffer* cmd, std::vector<vk::GeometryNV> const& geometries) {
  auto device = deviceContext->getVkDevice();
  auto& dispatch = deviceContext->getDispatch();

  vk::AccelerationStructureInfoNV asinfo;
  asinfo.type = vk::AccelerationStructureTypeNV::eBottomLevel;
  asinfo.flags = updatable
                   ? vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate
                   : vk::BuildAccelerationStructureFlagBitsNV::eAllowCompaction;
  asinfo.geometryCount = (uint)geometries.size();
  asinfo.pGeometries = geometries.data();
  asinfo.instanceCount = 0;

  vk::AccelerationStructureCreateInfoNV asci;
  asci.info = asinfo;

  structure = device.createAccelerationStructureNV(asci, nullptr, dispatch);

  // memory requirements info
  vk::AccelerationStructureMemoryRequirementsInfoNV memInfo;
  memInfo.accelerationStructure = structure;
  memInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eObject;
  auto memReqObject = device.getAccelerationStructureMemoryRequirementsNV(memInfo, dispatch);
  memInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eBuildScratch;
  auto memReqBuild = device.getAccelerationStructureMemoryRequirementsNV(memInfo, dispatch);
  memInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eUpdateScratch;
  auto memReqUpdate = device.getAccelerationStructureMemoryRequirementsNV(memInfo, dispatch);

  nei_log("Bottom BVH object {}kB scratch {}kB update {}kB", memReqObject.memoryRequirements.size/1024,
    memReqBuild.memoryRequirements.size/1024, memReqUpdate.memoryRequirements.size/1024);
  // allocate buffers
  buffer = new Buffer(deviceContext, uint(memReqObject.memoryRequirements.size), Buffer::Type::Raytracing);
  auto scratchSize = glm::max(memReqBuild.memoryRequirements.size, memReqUpdate.memoryRequirements.size);
  bufferScratch = new Buffer(deviceContext, uint(scratchSize), Buffer::Type::Raytracing);

  // bind memory to structure
  auto& allocation = buffer->getAllocation();
  vk::BindAccelerationStructureMemoryInfoNV bindInfo;
  bindInfo.accelerationStructure = structure;
  bindInfo.memory = allocation.memory;
  bindInfo.memoryOffset = allocation.offset;
  bindInfo.deviceIndexCount = 0;
  bindInfo.pDeviceIndices = nullptr;
  device.bindAccelerationStructureMemoryNV(bindInfo, dispatch);

  // build
  (**cmd).buildAccelerationStructureNV(asinfo, nullptr, 0, false, structure, nullptr, *bufferScratch, 0, dispatch);
  barrier(cmd);

  // handle
  device.getAccelerationStructureHandleNV(structure, sizeof(uint64), &handle, dispatch);
}

void AccelerationStructure::rebuild(CommandBuffer* cmd, std::vector<vk::GeometryNV> const& geometries) {
  auto device = deviceContext->getVkDevice();
  auto& dispatch = deviceContext->getDispatch();

  vk::AccelerationStructureInfoNV asinfo;
  asinfo.type = vk::AccelerationStructureTypeNV::eBottomLevel;
  asinfo.flags = updatable
    ? vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate
    : vk::BuildAccelerationStructureFlagBitsNV::eAllowCompaction;
  asinfo.geometryCount = (uint)geometries.size();
  asinfo.pGeometries = geometries.data();
  asinfo.instanceCount = 0;

  // build
  (**cmd).buildAccelerationStructureNV(asinfo, nullptr, 0, false, structure, nullptr, *bufferScratch, 0, dispatch);
  barrier(cmd);
}

void AccelerationStructure::update(CommandBuffer* cmd, std::vector<vk::GeometryNV> const& geometries) {
  nei_assert(updatable);
  auto device = deviceContext->getVkDevice();
  auto& dispatch = deviceContext->getDispatch();

  vk::AccelerationStructureInfoNV asinfo;
  asinfo.type = vk::AccelerationStructureTypeNV::eBottomLevel;
  asinfo.flags = vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate;
  asinfo.geometryCount = (uint)geometries.size();
  asinfo.pGeometries = geometries.data();
  asinfo.instanceCount = 0;

  // build
  (**cmd).buildAccelerationStructureNV(asinfo, nullptr, 0, true, structure, structure, *bufferScratch, 0, dispatch);
  barrier(cmd);
}

void AccelerationStructure::build(CommandBuffer* cmd, std::vector<vk::GeometryInstance> const& instances) {
  auto device = deviceContext->getVkDevice();
  auto& dispatch = deviceContext->getDispatch();

  vk::AccelerationStructureInfoNV asinfo;
  asinfo.type = vk::AccelerationStructureTypeNV::eTopLevel;
  asinfo.flags = {};
  if(updatable) asinfo.flags |= vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate;
  asinfo.instanceCount = (uint)instances.size();

  vk::AccelerationStructureCreateInfoNV asci;
  asci.info = asinfo;

  structure = device.createAccelerationStructureNV(asci, nullptr, dispatch);

  // memory requirements info
  vk::AccelerationStructureMemoryRequirementsInfoNV memInfo;
  memInfo.accelerationStructure = structure;
  memInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eObject;
  auto memReqObject = device.getAccelerationStructureMemoryRequirementsNV(memInfo, dispatch);
  memInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eBuildScratch;
  auto memReqBuild = device.getAccelerationStructureMemoryRequirementsNV(memInfo, dispatch);
  memInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eUpdateScratch;
  auto memReqUpdate = device.getAccelerationStructureMemoryRequirementsNV(memInfo, dispatch);

  nei_log("Top BVH object {}kB scratch {}kB update {}kB", memReqObject.memoryRequirements.size/1024,
    memReqBuild.memoryRequirements.size/1024, memReqUpdate.memoryRequirements.size/1024);

  buffer = new Buffer(deviceContext, uint(memReqObject.memoryRequirements.size), Buffer::Type::Raytracing);
  auto scratchSize = glm::max(memReqBuild.memoryRequirements.size, memReqUpdate.memoryRequirements.size);
  bufferScratch = new Buffer(deviceContext, uint(scratchSize), Buffer::Type::Raytracing);
  bufferInstances = new Buffer(deviceContext, uint(instances.size() * sizeof(vk::GeometryInstance)),
    Buffer::Type::Raytracing,
    Stream);

  bufferInstances->setData(instances.data(), uint(instances.size() * sizeof(vk::GeometryInstance)));

  auto& allocation = buffer->getAllocation();
  vk::BindAccelerationStructureMemoryInfoNV bindInfo;
  bindInfo.accelerationStructure = structure;
  bindInfo.memory = allocation.memory;
  bindInfo.memoryOffset = allocation.offset;
  bindInfo.deviceIndexCount = 0;
  bindInfo.pDeviceIndices = nullptr;
  device.bindAccelerationStructureMemoryNV(bindInfo, dispatch);

  (**cmd).buildAccelerationStructureNV(asinfo, *bufferInstances, 0, false, structure, nullptr, *bufferScratch, 0,
    dispatch);
  barrier(cmd);

  // handle
  device.getAccelerationStructureHandleNV(structure, sizeof(uint64), &handle, dispatch);
}

void AccelerationStructure::rebuild(CommandBuffer* cmd, std::vector<vk::GeometryInstance> const& instances) {
  auto device = deviceContext->getVkDevice();
  auto& dispatch = deviceContext->getDispatch();

  vk::AccelerationStructureInfoNV asinfo;
  asinfo.type = vk::AccelerationStructureTypeNV::eTopLevel;
  asinfo.flags = {};
  if (updatable) asinfo.flags |= vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate;
  asinfo.instanceCount = (uint)instances.size();
  
  (**cmd).buildAccelerationStructureNV(asinfo, *bufferInstances, 0, false, structure, nullptr, *bufferScratch, 0,
    dispatch);
  barrier(cmd);
}

void AccelerationStructure::update(CommandBuffer* cmd, std::vector<vk::GeometryInstance> const& instances) {
  nei_assert(updatable);
  auto device = deviceContext->getVkDevice();
  auto& dispatch = deviceContext->getDispatch();

  vk::AccelerationStructureInfoNV asinfo;
  asinfo.type = vk::AccelerationStructureTypeNV::eTopLevel;
  asinfo.flags = vk::BuildAccelerationStructureFlagBitsNV::eAllowUpdate;
  asinfo.instanceCount = (uint)instances.size();

  // skip instance data copy - it is always one instance with identity transform

  (**cmd).buildAccelerationStructureNV(asinfo, *bufferInstances, 0, true, structure, structure, *bufferScratch, 0,
    dispatch);
  barrier(cmd);
}

void AccelerationStructure::compact() {
  nei_assert(!updatable);

  auto device = deviceContext->getVkDevice();
  auto& dispatch = deviceContext->getDispatch();

  vk::QueryPoolCreateInfo qpci;
  qpci.queryType = vk::QueryType::eAccelerationStructureCompactedSizeNV;
  qpci.queryCount = 1;
  auto pool = device.createQueryPool(qpci);

  Ptr cmd = deviceContext->getSingleUseCommandBuffer();
  cmd->begin();
  (**cmd).resetQueryPool(pool, 0, 1);
  (**cmd).writeAccelerationStructuresPropertiesNV(1, &structure, vk::QueryType::eAccelerationStructureCompactedSizeNV,
    pool, 0, dispatch);

  cmd->end();
  cmd->submit();

  uint64 data;
  device.getQueryPoolResults(pool, 0, 1, sizeof(uint64), &data, sizeof(uint64),
    vk::QueryResultFlagBits::e64 | vk::QueryResultFlagBits::eWait);
  device.destroyQueryPool(pool);

  nei_log("BVH Bottom compacted {} kB", data/1024);

  vk::AccelerationStructureCreateInfoNV info;
  info.compactedSize = data;
  info.info.type = vk::AccelerationStructureTypeNV::eBottomLevel;
  info.info.flags = {};

  auto compacted = device.createAccelerationStructureNV(info, nullptr, dispatch);

  vk::AccelerationStructureMemoryRequirementsInfoNV memInfo;
  memInfo.accelerationStructure = structure;
  memInfo.type = vk::AccelerationStructureMemoryRequirementsTypeNV::eObject;
  auto memReqObject = device.getAccelerationStructureMemoryRequirementsNV(memInfo, dispatch);

  auto compactedBuffer = new Buffer(deviceContext, uint(memReqObject.memoryRequirements.size),
    Buffer::Type::Raytracing);

  auto& allocation = compactedBuffer->getAllocation();
  vk::BindAccelerationStructureMemoryInfoNV bindInfo;
  bindInfo.accelerationStructure = compacted;
  bindInfo.memory = allocation.memory;
  bindInfo.memoryOffset = allocation.offset;
  bindInfo.deviceIndexCount = 0;
  bindInfo.pDeviceIndices = nullptr;
  device.bindAccelerationStructureMemoryNV(bindInfo, dispatch);

  cmd->begin();
  (**cmd).copyAccelerationStructureNV(compacted, structure, vk::CopyAccelerationStructureModeNV::eCompact, dispatch);
  cmd->end();
  cmd->submit();

  device.destroyAccelerationStructureNV(structure, nullptr, dispatch);

  buffer = compactedBuffer;
  structure = compacted;
  device.getAccelerationStructureHandleNV(structure, sizeof(uint64), &handle, dispatch);
}

void AccelerationStructure::barrier(CommandBuffer* cmd) {
  cmd->memoryBarrier(vk::PipelineStageFlagBits::eAccelerationStructureBuildNV,
    vk::PipelineStageFlagBits::eAccelerationStructureBuildNV,
    vk::AccessFlagBits::eAccelerationStructureReadNV | vk::AccessFlagBits::
    eAccelerationStructureWriteNV,
    vk::AccessFlagBits::eAccelerationStructureReadNV | vk::AccessFlagBits::
    eAccelerationStructureWriteNV);
}
