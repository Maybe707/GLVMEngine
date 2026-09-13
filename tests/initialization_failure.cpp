#include <vulkan/vulkan_core.h>

// Fail after device, swapchain, render passes, layouts and shader modules exist.
extern "C" VkResult __wrap_vkCreateGraphicsPipelines(
    VkDevice, VkPipelineCache, uint32_t, const VkGraphicsPipelineCreateInfo*,
    const VkAllocationCallbacks*, VkPipeline*) {
    return VK_ERROR_INITIALIZATION_FAILED;
}
