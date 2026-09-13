#pragma once
#include "GraphicAPI/RenderData.hpp"
namespace GLVM::core {
inline constexpr uint32_t MAX_TEXTURES = 18;
void VkConfigInitializer(VulkanResources& resources);
}
