#pragma once
#include "VkStructs.hpp"
#include <deque>
namespace GLVM::core {
// Per-renderer state. Descriptor pointers borrow stable records owned here;
// destruction frees CPU metadata even if GPU initialization throws.
struct VulkanResources final {
    DescriptorSet descriptorSetsConfig[32]{};
    DescriptorBinding descriptorBindingsConfig[32]{};
    Pipeline pipelineConfigs[32]{};
    RenderPass renderPassConfigs[32]{};
    vector<VkDescriptorSet> descriptorSetsChunks;
    vector<VkRenderPass> renderPasses;
    vector<Descriptor> GPUDescriptors;
    std::deque<GPUBuffer> buffers;
    std::deque<VK_Image> images;
    VulkanResources() = default;
    VulkanResources(const VulkanResources&) = delete;
    VulkanResources& operator=(const VulkanResources&) = delete;
};
}
