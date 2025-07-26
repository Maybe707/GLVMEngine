#ifndef RENDER_DATA
#define RENDER_DATA

#include "Vector.hpp"
#include <vulkan/vulkan_core.h>


extern GLVM::core::vector<GLVM::core::vector<VkDescriptorSet>> descriptorSetsChunks;
extern GLVM::core::vector<VkRenderPass> renderPasses;

#endif
