#pragma once
#include <vulkan/vulkan_core.h>

namespace GLVM::core {
// Shader modules are temporary pipeline inputs, released on both success and
// failure. The device is borrowed and must outlive this scope.
class UniqueShaderModule final {
public:
    explicit UniqueShaderModule(VkDevice device) : device_(device) {}
    ~UniqueShaderModule() { if (handle_) vkDestroyShaderModule(device_, handle_, nullptr); }
    UniqueShaderModule(const UniqueShaderModule&) = delete;
    UniqueShaderModule& operator=(const UniqueShaderModule&) = delete;
    void reset(VkShaderModule handle) {
        if (handle_) vkDestroyShaderModule(device_, handle_, nullptr);
        handle_ = handle;
    }
    VkShaderModule get() const noexcept { return handle_; }
private:
    VkDevice device_;
    VkShaderModule handle_ = VK_NULL_HANDLE;
};
}
