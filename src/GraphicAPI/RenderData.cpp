#include "GraphicAPI/RenderData.hpp"

namespace GLVM::core {
	GLVM::core::vector<vector<VkDescriptorSet>> descriptorSetsChunks;
	GLVM::core::vector<VkRenderPass> renderPasses;
	GLVM::core::vector<Descriptor> GPUDescriptors;
} // namespace GLVM::core
