#ifndef RENDER_DATA
#define RENDER_DATA

#include "Vector.hpp"
#include <vulkan/vulkan_core.h>
#include "VkStructs.hpp"
namespace GLVM::core {
	extern vector<VkDescriptorSet> descriptorSetsChunks;
	extern GLVM::core::vector<VkRenderPass> renderPasses;
	extern GLVM::core::vector<Descriptor> GPUDescriptors;
} // namespace GLVM::core

#endif
