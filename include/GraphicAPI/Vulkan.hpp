#ifndef VULKAN_RENDERER
#define VULKAN_RENDERER

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <optional>
#include <set>

#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "IRenderer.hpp"
#include "Texture.hpp"
#include "VectorContainer.hpp"
#include "VertexMath.hpp"
#include "TextureManager.hpp"
#include "ComponentManager.hpp"
#include "WavefrontObjParser.hpp"

#ifdef __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
#include "vulkan/vulkan.h"
#include <X11/Xlib.h>
#include "vulkan/vulkan_xlib.h"
#include "vulkan/vulkan_core.h"
#include "UnixApi/WindowX.hpp"
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
#include "../../../Vulkan-Headers/include/vulkan/vulkan.h"
#include "../../../Vulkan-Headers/include/vulkan/vulkan_core.h"
#include "WinApi/WindowWin.hpp"
//#include "WinApi/winVk.hpp"
#endif

// #ifdef NDEBUG
// #define WNDCLASS 0xc018
// #else
// #define WNDCLASS "static"
// #endif

namespace GLVM::core
{
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    struct Texture {
        VkDeviceSize textureSize_;
        unsigned char* textureData_;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    struct Vertex {
        vec3 pos;
        vec3 color;
        vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }
    };

    struct UniformBufferObject {
        alignas(16) mat4 model;
        alignas(16) mat4 view;
        alignas(16) mat4 proj;
    };

    class CVertexObject
    {
    public:
        const char* k_cPath_;
        
        std::vector<float> aVertex_ {
        };


    };
    
//     const std::vector<Vertex> vertices = {
//         {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//         {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//         {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//         {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

//         {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//         {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//         {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//         {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

//         {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//         {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//         {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//         {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

//         {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//         {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//         {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//         {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

//         {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//         {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//         {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//         {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

//         {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//         {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//         {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//         {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
//     };
    
// // Выдай в шейдере output = vec4(uv.xy, 0,1) чтобы было наглядно.

//     const std::vector<uint16_t> indices = {
//         0, 1, 2, 2, 3, 0,
//         4, 5, 6, 6, 7, 4,
//         8, 9, 10, 10, 11, 8,
//         12, 13, 14, 14, 15, 12,
//         16, 17, 18, 18, 19, 16,
//         20, 21, 22, 22, 23, 20
//     };

    const std::vector<Vertex> vertices = {
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    };
    
// Выдай в шейдере output = vec4(uv.xy, 0,1) чтобы было наглядно.

    const std::vector<uint16_t> indices = {
        4, 2, 0,
        2, 7, 3,
        6, 5, 7,
        1, 7, 5,
        0, 3, 1,
        4, 1, 5,
        4, 6, 2,
        2, 6, 7,
        6, 4, 5,
        1, 3, 7,
        0, 2, 3,
        4, 0, 1
    };

    const std::vector<Vertex> hudVertices = {
        {{0.1f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.1f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.1f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.1f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{0.5f, -0.1f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.1f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.1f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.1f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    };
    
// Выдай в шейдере output = vec4(uv.xy, 0,1) чтобы было наглядно.

    const std::vector<uint16_t> hudIndices = {
        0, 1, 2, 2, 1, 3,
        4, 5, 6, 6, 5, 7
    };
    
    class CVulkanRenderer : public IRenderer {
    public:
        std::vector<ecs::CTexture> initializeTextureData_;
        std::vector<ecs::CTexture> texture_load_data_;
        std::vector<ecs::CTexture> hudTexture_load_data_;
        std::vector<ecs::components::transform> transform_data_;
        std::vector<const char*> pathsArray_;
        std::vector<std::vector<core::Vertex>> aVertices_;
        std::vector<std::vector<uint16_t>> aIndices_;

        const char* vertShaderMain_ = "../VKshaders/shaders/vert.spv";
        const char* fragShaderMain_ = "../VKshaders/shaders/frag.spv";

        const char* vertShaderHUD_ = "../VKshaders/hudShaders/vert.spv";
        const char* fragShaderHUD_ = "../VKshaders/hudShaders/frag.spv";
        
        unsigned int texturePool_;
        
#ifdef VK_USE_PLATFORM_XLIB_KHR
        GLVM::core::CWindowX Window;
#endif
    
#ifdef VK_USE_PLATFORM_WIN32_KHR
        GLVM::core::CWindowWin Window;
#endif
        
        CVulkanRenderer(std::vector<ecs::CTexture> _texture_data, std::vector<ecs::CTexture> _initializeHUDTextureData);
        ~CVulkanRenderer();

        void createTextureImage();
        void recreateSwapChain();
        void draw() override;
        void loadWavefrontObj() override;
        void SetTextureData(std::vector<ecs::CTexture>& _texture_data, std::vector<ecs::CTexture>& _hud_texture_data) override;
        void SetMeshData(std::vector<const char*> _pathsArray) override;
        void SetViewMatrix(mat4 _viewMatrix) override;
        void SetProjectionMatrix(mat4 _projectionMatrix) override;
        void run() override;
    
    private:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
		mat4 viewMatrix;
		mat4 projectionMatrix;

#ifdef VK_USE_PLATFORM_XLIB_KHR
        VkXlibSurfaceCreateInfoKHR createXlibSurfaceInfo;
#endif
    
#ifdef VK_USE_PLATFORM_WIN32_KHR
        VkWin32SurfaceCreateInfoKHR createWin32SurfaceInfo;
#endif
    
        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSetLayout descriptorSetLayoutHUD;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkPipelineLayout pipelineLayoutHUD;
        VkPipeline graphicsPipelineHUD;

        VkCommandPool commandPool;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        std::vector<VkImage> textureImages;
        std::vector<VkDeviceMemory> textureImageMemories;
        std::vector<VkImageView> textureImageViews;
        std::vector<VkSampler> textureSamplers;

        // VkBuffer vertexBuffer;
        // VkDeviceMemory vertexBufferMemory;
        // VkBuffer indexBuffer;
        // VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> vertexBufferContainer;
        std::vector<VkDeviceMemory> vertexBufferMemoryContainer;
        std::vector<VkBuffer> indexBufferContainer;
        std::vector<VkDeviceMemory> indexBufferMemoryContaner;

        VkBuffer hudVertexBuffer;
        VkDeviceMemory hudVertexBufferMemory;
        VkBuffer hudIndexBuffer;
        VkDeviceMemory hudIndexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        uint32_t currentFrame = 0;

        bool framebufferResized = false;

        void initWindow();
        void initVulkan();
        void cleanupSwapChain();
        void cleanup();
        void createInstance();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createDescriptorSetLayout(VkDescriptorSetLayout& _descriptorSetLayout);
        void createGraphicsPipeline(VkPipeline& _graphicsPipeline, VkPipelineLayout& _pipelineLayout, VkDescriptorSetLayout& _descriptorSetLayout, const char* _vertShader, const char* _fragShader);
        void createFramebuffers();
        void createCommandPool();
        void createDepthResources();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();
        bool hasStencilComponent(VkFormat format);
        void createTextureImageView();
        void createTextureSampler();
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void createVertexBuffer(VkBuffer& _vertexBuffer, VkDeviceMemory& _vertexBufferMemory, const std::vector<Vertex>& _vertices);
        void createIndexBuffer(VkBuffer& _indexBuffer, VkDeviceMemory& _indexBufferMemory, const std::vector<uint16_t>& _indices);
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void createCommandBuffers();
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void createSyncObjects();
        void updateUniformBuffer(uint32_t currentImage, ecs::components::transform _transformComponent);
        void drawFrame();
        VkShaderModule createShaderModule(const std::vector<char>& code);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        static std::vector<char> readFile(const std::string& filename);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    };

};

#endif
