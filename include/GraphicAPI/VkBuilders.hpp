#ifndef VK_BUILDERS
#define VK_BUILDERS

#include"RenderData.hpp"

namespace GLVM::core {
	void descriptorSetBuilder(VulkanResources& resources);
	void pipelineBuilder(VulkanResources& resources);
	void renderPassesBuilder(VulkanResources& resources);
}; // GLVM::core


#endif
