#ifndef VK_DEBUG_UTILS
#define VK_DEBUG_UTILS

#include <string>
#include "VkStructs.hpp"
#include "ToString.hpp"
#include "RenderConfig.hpp"

namespace GLVM::core::vkDebugUtils
{
	VkResult SetDebugObjectName(VkDevice device, const VkDebugUtilsObjectNameInfoEXT* objectNameInfo);
	void setImageDebugObjectName( VkDevice device, VK_Image image, std::string imageName );
	void setPipelineDebugObjectName( VkDevice device, VkPipeline pipeline, std::string pipelineName );
	void setDescriptorSetObjectName( VkDevice device, VkDescriptorSet descriptorSet, std::string descriptorSetName, unsigned int index );
	void setDebugObjectNames( VkDevice device, const std::vector<VkBuffer>& vertexBufferContainer, const std::vector<VkBuffer>& indexBufferContainer, const GLVM::core::vector<Descriptor>& GPUDescriptors,
							  const std::vector<unsigned int>& fontIndicesContainer, const std::vector<VkBuffer>& fontVertexBufferContainer, const std::vector<VkBuffer>& fontIndexBufferContainer );
};

#endif
