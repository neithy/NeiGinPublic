#pragma once

#include "Format.h"

bool Nei::Vu::isDepthFormat(vk::Format format) {
  switch (format) {
  case vk::Format::eD16Unorm:
  case vk::Format::eX8D24UnormPack32:
  case vk::Format::eD32Sfloat:
  case vk::Format::eS8Uint:
  case vk::Format::eD16UnormS8Uint:
  case vk::Format::eD24UnormS8Uint:
  case vk::Format::eD32SfloatS8Uint:
    return true;
  default:
    return false;
  }
}

int Nei::Vu::formatSize(vk::Format format) {
  switch (format) {

  case vk::Format::eUndefined:
    return 0;
  case vk::Format::eR4G4UnormPack8: break;
  case vk::Format::eR4G4B4A4UnormPack16: break;
  case vk::Format::eB4G4R4A4UnormPack16: break;
  case vk::Format::eR5G6B5UnormPack16: break;
  case vk::Format::eB5G6R5UnormPack16: break;
  case vk::Format::eR5G5B5A1UnormPack16: break;
  case vk::Format::eB5G5R5A1UnormPack16: break;
  case vk::Format::eA1R5G5B5UnormPack16: break;
    //
  case vk::Format::eR8Unorm:
  case vk::Format::eR8Snorm:
  case vk::Format::eR8Uscaled:
  case vk::Format::eR8Sscaled:
  case vk::Format::eR8Uint:
  case vk::Format::eR8Sint:
  case vk::Format::eR8Srgb:
    return 1;
  case vk::Format::eR8G8Unorm:
  case vk::Format::eR8G8Snorm:
  case vk::Format::eR8G8Uscaled:
  case vk::Format::eR8G8Sscaled:
  case vk::Format::eR8G8Uint:
  case vk::Format::eR8G8Sint:
  case vk::Format::eR8G8Srgb:
    return 2;
  case vk::Format::eR8G8B8Unorm:
  case vk::Format::eR8G8B8Snorm:
  case vk::Format::eR8G8B8Uscaled:
  case vk::Format::eR8G8B8Sscaled:
  case vk::Format::eR8G8B8Uint:
  case vk::Format::eR8G8B8Sint:
  case vk::Format::eR8G8B8Srgb:
  case vk::Format::eB8G8R8Unorm:
  case vk::Format::eB8G8R8Snorm:
  case vk::Format::eB8G8R8Uscaled:
  case vk::Format::eB8G8R8Sscaled:
  case vk::Format::eB8G8R8Uint:
  case vk::Format::eB8G8R8Sint:
  case vk::Format::eB8G8R8Srgb:
    return 3;
  case vk::Format::eR8G8B8A8Unorm:
  case vk::Format::eR8G8B8A8Snorm:
  case vk::Format::eR8G8B8A8Uscaled:
  case vk::Format::eR8G8B8A8Sscaled:
  case vk::Format::eR8G8B8A8Uint:
  case vk::Format::eR8G8B8A8Sint:
  case vk::Format::eR8G8B8A8Srgb:
  case vk::Format::eB8G8R8A8Unorm:
  case vk::Format::eB8G8R8A8Snorm:
  case vk::Format::eB8G8R8A8Uscaled:
  case vk::Format::eB8G8R8A8Sscaled:
  case vk::Format::eB8G8R8A8Uint:
  case vk::Format::eB8G8R8A8Sint:
  case vk::Format::eB8G8R8A8Srgb:
    return 4;
  case vk::Format::eA8B8G8R8UnormPack32: 
  case vk::Format::eA8B8G8R8SnormPack32: 
  case vk::Format::eA8B8G8R8UscaledPack32: 
  case vk::Format::eA8B8G8R8SscaledPack32: 
  case vk::Format::eA8B8G8R8UintPack32: 
  case vk::Format::eA8B8G8R8SintPack32: 
  case vk::Format::eA8B8G8R8SrgbPack32: 
  case vk::Format::eA2R10G10B10UnormPack32: 
  case vk::Format::eA2R10G10B10SnormPack32: 
  case vk::Format::eA2R10G10B10UscaledPack32: 
  case vk::Format::eA2R10G10B10SscaledPack32: 
  case vk::Format::eA2R10G10B10UintPack32: 
  case vk::Format::eA2R10G10B10SintPack32: 
  case vk::Format::eA2B10G10R10UnormPack32: 
  case vk::Format::eA2B10G10R10SnormPack32: 
  case vk::Format::eA2B10G10R10UscaledPack32: 
  case vk::Format::eA2B10G10R10SscaledPack32: 
  case vk::Format::eA2B10G10R10UintPack32: 
  case vk::Format::eA2B10G10R10SintPack32: 
    return 4;
  case vk::Format::eR16Unorm: 
  case vk::Format::eR16Snorm: 
  case vk::Format::eR16Uscaled: 
  case vk::Format::eR16Sscaled: 
  case vk::Format::eR16Uint: 
  case vk::Format::eR16Sint: 
  case vk::Format::eR16Sfloat: 
    return 2;
  case vk::Format::eR16G16Unorm: 
  case vk::Format::eR16G16Snorm: 
  case vk::Format::eR16G16Uscaled: 
  case vk::Format::eR16G16Sscaled: 
  case vk::Format::eR16G16Uint: 
  case vk::Format::eR16G16Sint: 
  case vk::Format::eR16G16Sfloat: 
    return 4;
  case vk::Format::eR16G16B16Unorm: 
  case vk::Format::eR16G16B16Snorm: 
  case vk::Format::eR16G16B16Uscaled: 
  case vk::Format::eR16G16B16Sscaled: 
  case vk::Format::eR16G16B16Uint: 
  case vk::Format::eR16G16B16Sint: 
  case vk::Format::eR16G16B16Sfloat: 
    return 6;
  case vk::Format::eR16G16B16A16Unorm: 
  case vk::Format::eR16G16B16A16Snorm: 
  case vk::Format::eR16G16B16A16Uscaled: 
  case vk::Format::eR16G16B16A16Sscaled: 
  case vk::Format::eR16G16B16A16Uint: 
  case vk::Format::eR16G16B16A16Sint: 
  case vk::Format::eR16G16B16A16Sfloat: 
    return 8;
  case vk::Format::eR32Uint: 
  case vk::Format::eR32Sint: 
  case vk::Format::eR32Sfloat: 
    return 4;
  case vk::Format::eR32G32Uint: 
  case vk::Format::eR32G32Sint: 
  case vk::Format::eR32G32Sfloat: 
    return 8;
  case vk::Format::eR32G32B32Uint: 
  case vk::Format::eR32G32B32Sint: 
  case vk::Format::eR32G32B32Sfloat: 
    return 12;
  case vk::Format::eR32G32B32A32Uint: 
  case vk::Format::eR32G32B32A32Sint: 
  case vk::Format::eR32G32B32A32Sfloat: 
    return 16;
  case vk::Format::eR64Uint: 
  case vk::Format::eR64Sint: 
  case vk::Format::eR64Sfloat: 
    return 8;
  case vk::Format::eR64G64Uint: 
  case vk::Format::eR64G64Sint: 
  case vk::Format::eR64G64Sfloat: 
    return 16;
  case vk::Format::eR64G64B64Uint: 
  case vk::Format::eR64G64B64Sint: 
  case vk::Format::eR64G64B64Sfloat: 
    return 24;
  case vk::Format::eR64G64B64A64Uint: 
  case vk::Format::eR64G64B64A64Sint: 
  case vk::Format::eR64G64B64A64Sfloat: 
    return 32;
  case vk::Format::eB10G11R11UfloatPack32: 
  case vk::Format::eE5B9G9R9UfloatPack32: 
    return 4;
  case vk::Format::eD16Unorm: 
    return 2;
  case vk::Format::eX8D24UnormPack32: 
    return 4;
  case vk::Format::eD32Sfloat: 
    return 4;
  case vk::Format::eS8Uint: 
    return 1;
  case vk::Format::eD16UnormS8Uint: 
    return 3;
  case vk::Format::eD24UnormS8Uint: 
    return 4;
  case vk::Format::eD32SfloatS8Uint: 
    return 5;
  case vk::Format::eBc1RgbUnormBlock: break;
  case vk::Format::eBc1RgbSrgbBlock: break;
  case vk::Format::eBc1RgbaUnormBlock: break;
  case vk::Format::eBc1RgbaSrgbBlock: break;
  case vk::Format::eBc2UnormBlock: break;
  case vk::Format::eBc2SrgbBlock: break;
  case vk::Format::eBc3UnormBlock: break;
  case vk::Format::eBc3SrgbBlock: break;
  case vk::Format::eBc4UnormBlock: break;
  case vk::Format::eBc4SnormBlock: break;
  case vk::Format::eBc5UnormBlock: break;
  case vk::Format::eBc5SnormBlock: break;
  case vk::Format::eBc6HUfloatBlock: break;
  case vk::Format::eBc6HSfloatBlock: break;
  case vk::Format::eBc7UnormBlock: break;
  case vk::Format::eBc7SrgbBlock: break;
  case vk::Format::eEtc2R8G8B8UnormBlock: break;
  case vk::Format::eEtc2R8G8B8SrgbBlock: break;
  case vk::Format::eEtc2R8G8B8A1UnormBlock: break;
  case vk::Format::eEtc2R8G8B8A1SrgbBlock: break;
  case vk::Format::eEtc2R8G8B8A8UnormBlock: break;
  case vk::Format::eEtc2R8G8B8A8SrgbBlock: break;
  case vk::Format::eEacR11UnormBlock: break;
  case vk::Format::eEacR11SnormBlock: break;
  case vk::Format::eEacR11G11UnormBlock: break;
  case vk::Format::eEacR11G11SnormBlock: break;
  case vk::Format::eAstc4x4UnormBlock: break;
  case vk::Format::eAstc4x4SrgbBlock: break;
  case vk::Format::eAstc5x4UnormBlock: break;
  case vk::Format::eAstc5x4SrgbBlock: break;
  case vk::Format::eAstc5x5UnormBlock: break;
  case vk::Format::eAstc5x5SrgbBlock: break;
  case vk::Format::eAstc6x5UnormBlock: break;
  case vk::Format::eAstc6x5SrgbBlock: break;
  case vk::Format::eAstc6x6UnormBlock: break;
  case vk::Format::eAstc6x6SrgbBlock: break;
  case vk::Format::eAstc8x5UnormBlock: break;
  case vk::Format::eAstc8x5SrgbBlock: break;
  case vk::Format::eAstc8x6UnormBlock: break;
  case vk::Format::eAstc8x6SrgbBlock: break;
  case vk::Format::eAstc8x8UnormBlock: break;
  case vk::Format::eAstc8x8SrgbBlock: break;
  case vk::Format::eAstc10x5UnormBlock: break;
  case vk::Format::eAstc10x5SrgbBlock: break;
  case vk::Format::eAstc10x6UnormBlock: break;
  case vk::Format::eAstc10x6SrgbBlock: break;
  case vk::Format::eAstc10x8UnormBlock: break;
  case vk::Format::eAstc10x8SrgbBlock: break;
  case vk::Format::eAstc10x10UnormBlock: break;
  case vk::Format::eAstc10x10SrgbBlock: break;
  case vk::Format::eAstc12x10UnormBlock: break;
  case vk::Format::eAstc12x10SrgbBlock: break;
  case vk::Format::eAstc12x12UnormBlock: break;
  case vk::Format::eAstc12x12SrgbBlock: break;
  case vk::Format::eG8B8G8R8422Unorm: break;
  case vk::Format::eB8G8R8G8422Unorm: break;
  case vk::Format::eG8B8R83Plane420Unorm: break;
  case vk::Format::eG8B8R82Plane420Unorm: break;
  case vk::Format::eG8B8R83Plane422Unorm: break;
  case vk::Format::eG8B8R82Plane422Unorm: break;
  case vk::Format::eG8B8R83Plane444Unorm: break;
  case vk::Format::eR10X6UnormPack16: break;
  case vk::Format::eR10X6G10X6Unorm2Pack16: break;
  case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16: break;
  case vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16: break;
  case vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16: break;
  case vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16: break;
  case vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16: break;
  case vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16: break;
  case vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16: break;
  case vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16: break;
  case vk::Format::eR12X4UnormPack16: break;
  case vk::Format::eR12X4G12X4Unorm2Pack16: break;
  case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16: break;
  case vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16: break;
  case vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16: break;
  case vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16: break;
  case vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16: break;
  case vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16: break;
  case vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16: break;
  case vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16: break;
  case vk::Format::eG16B16G16R16422Unorm: break;
  case vk::Format::eB16G16R16G16422Unorm: break;
  case vk::Format::eG16B16R163Plane420Unorm: break;
  case vk::Format::eG16B16R162Plane420Unorm: break;
  case vk::Format::eG16B16R163Plane422Unorm: break;
  case vk::Format::eG16B16R162Plane422Unorm: break;
  case vk::Format::eG16B16R163Plane444Unorm: break;
  case vk::Format::ePvrtc12BppUnormBlockIMG: break;
  case vk::Format::ePvrtc14BppUnormBlockIMG: break;
  case vk::Format::ePvrtc22BppUnormBlockIMG: break;
  case vk::Format::ePvrtc24BppUnormBlockIMG: break;
  case vk::Format::ePvrtc12BppSrgbBlockIMG: break;
  case vk::Format::ePvrtc14BppSrgbBlockIMG: break;
  case vk::Format::ePvrtc22BppSrgbBlockIMG: break;
  case vk::Format::ePvrtc24BppSrgbBlockIMG: break;
  }
  nei_error("NYI");
  return 0;
}

vec4 Nei::Vu::colorF(u8vec4 c) {
  return vec4(c) / 255.0f;
}

u8vec4 Nei::Vu::color(vec4 c) {
  return u8vec4(clamp(c, 0.f, 1.f) * 255.f);
}
