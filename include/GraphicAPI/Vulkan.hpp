// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef VULKAN_RENDERER_HG
#define VULKAN_RENDERER_HG

#include <cstdint>
#include <iostream>
#include <fstream>
#include <new>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <optional>
#include <set>
#include <cmath>
#include <vulkan/vulkan_core.h>

#include "Components/ItemComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Texture.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include "TextureManager.hpp"
#include "ComponentManager.hpp"
#include "WavefrontObjParser.hpp"
#include "MeshManager.hpp"
#include "Globals.hpp"
#include "VkStructs.hpp"
#include "ToString.hpp"
#include "JsonParser.hpp"
#include "ShaderStructs.hpp"
#include "Components/FontComponent.hpp"

#ifdef __linux__
//#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XCB_KHR
//#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_wayland.h"
#include "UnixApi/WindowWaylandVulkan.hpp"
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
#include "vulkan/vulkan.h"
#include <xcb/xcb.h>
#include "vulkan/vulkan_xcb.h"
#include "vulkan/vulkan_core.h"
#include "UnixApi/WindowXCBVulkan.hpp"
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
#include "vulkan/vulkan.h"
#include <X11/Xlib.h>
#include "vulkan/vulkan_xlib.h"
#include "vulkan/vulkan_core.h"
#include "UnixApi/WindowXVulkan.hpp"
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "WinApi/WindowWinVulkan.hpp"
#endif

namespace GLVM::core
{
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const int MAX_FRAMES_IN_FLIGHT = 2;
//#define NDEBUG
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_shader_non_semantic_info"
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
		vec4 joinIndices;
		vec4 weights;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescription.stride = sizeof(Vertex);

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

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

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, joinIndices);

            attributeDescriptions[4].binding = 0;
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[4].offset = offsetof(Vertex, weights);
			
            return attributeDescriptions;
        }
    };

	struct VK_Image {
		VkImage image = {};
		VkDeviceMemory deviceMemory = {};
		std::vector<VkImageView> views = {};
		VkImageViewType viewType = {};
		VkImageCreateFlags createFlags = {};
		VkMemoryPropertyFlags memoryPropertyFlags = {};
		VkImageUsageFlags usageFlags = {};
		VkImageAspectFlags aspectFlags = {};
		VkFormat format = {};
		VkImageTiling tiling = {};
		VkSampler sampler = {};
		VkComponentSwizzle red = {};
		VkComponentSwizzle green = {};
		VkComponentSwizzle blue = {};
		VkComponentSwizzle alpha = {};
		uint32_t arrayLayers = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};
	
	struct Descriptor {
		VkDescriptorType       vkType;
		DescriptorsTypes       type;
		core::vector<u32> binding;
		VkShaderStageFlags     shaderStageFlag;
		VkDescriptorSetLayout  setLayout;
		core::vector<u32>               descriptorsNumber;

		// std::vector<VkBuffer> uniformBuffers;
		// std::vector<VkDeviceMemory> uniformBuffersMemory;
		
		std::vector<VK_Image> textureImages;
	};

	struct Pipeline {
		core::vector<Descriptor> descriptors;
		unsigned int globalDescriptorsNumber = 0;
		unsigned int uboDescriptorsNumber = 0;
		unsigned int combinedImageSamplersNumber = 0;
		VkPipeline  pipeline;
		VkPipelineLayout pipelineLayout;
		const char* vertShader = nullptr;
		const char* fragShader = nullptr;
		VkVertexInputBindingDescription bindingDescription;
		std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions;

		void addDescriptor(VkDescriptorType vkType, DescriptorsTypes type, VkShaderStageFlags shaderStageFlag,
						   core::vector<u32> descriptorsNumbers, core::vector<uint32_t> bindings) {
			if (vkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				descriptors.Push({vkType, type, bindings, shaderStageFlag, VkDescriptorSetLayout(), descriptorsNumbers, {}});
				++uboDescriptorsNumber;
			} else if (vkType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
				descriptors.Push({vkType, type, bindings, shaderStageFlag, VkDescriptorSetLayout(), descriptorsNumbers, {}});
				++combinedImageSamplersNumber;
			} else {
				assert(!"unreachable");
			}
		}

		core::vector<u32> getBindingOfDescriptor(DescriptorsTypes type) const {
			for ( unsigned int i = 0; i < descriptors.GetSize(); ++i ) {
				if ( type == descriptors[i].type )
					return descriptors[i].binding;
			}

			core::vector<u32> empty;
			return empty;
		}
	};

    class CVulkanRenderer {
    public:
		bool print = true;
		unsigned int actorsNumber = 0;
		core::vector<unsigned int> entitiesCollectionLinked__Trn_Mat_Mes_Act;
		core::vector<unsigned int> entitiesCollectionLinked__Trn_PoL_Mes_Act;

		char glyphs[128]  = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
			'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
			'p', 'q', 'r', 's', 't', 'u', 'v', 'y', 'x', 'y', 'z', '0', '1', '2', '3',
			'4', '5', '6', '7', '8', '9', '.', ',', '"', '"', '\'', '\'', '"', '"', '\'', '\'',
			'?', '!', '_', '$', '(', ')', '+', '-', '/', ':', ';', '<', '>', '=', '[', ']', '\\'};
		const std::vector<Vertex> padding[128];                                     ///< FIXME: Some gabage here that needs maybe for aligh
		const std::vector<uint32_t> symbol_g_indices = {
			0, 1, 2,
			2, 1, 3
		};

        std::vector<ecs::Texture> initializeTextureData_;
        std::vector<const char*> pathsArray_;
		core::vector<const char*> pathsGLTF_;
        std::vector<core::vector<core::Vertex>> levelGeneratedVertices;
        std::vector<std::vector<uint32_t>> levelGeneratedIndices;
		
        std::vector<core::vector<core::Vertex>> aVertices_;
        std::vector<std::vector<uint32_t>> aIndices_;                          ///< wavefront.obj indices
		std::vector<std::vector<float>> aVertexesTemp_;                        ///< gltf indices
		std::vector<float> highest_gltf_Y;                                     /// highest gltf y
		MeshAxisLimitingValues meshAxisLimitingValues;                         /// keep axis liniting values for every exis per mesh in current iteration while initializing wavefrontobj and gltf
		core::vector<core::vector<core::vector<mat4>>> jointMatricesPerMesh;
		core::vector<core::vector<float>> frames;
		bool isInventoryOpened = false;
		vec3 forward;
		float hud_screen_x = 0.0f;
		float hud_screen_y;

		float fYaw   = -90.0f;
        float fPitch = 0.0f;
		float prev_Y = 0.0f;
		float current_Y = 0.0f;
		float prev_X = 0.0f;
		float current_X = 0.0f;

        const char* vertShaderMain_ = "../VKshaders/mainRendererShaders/vert.spv";
        const char* fragShaderMain_ = "../VKshaders/mainRendererShaders/frag.spv";

        const char* vertShaderFlatShadowMap = "../VKshaders/flatShadowMapShaders/vertFlatShadowMap.spv";
        const char* fragShaderDirectionalLightShadowMap = "../VKshaders/flatShadowMapShaders/fragFlatShadowMap.spv";

        const char* vertShaderCubeShadowMap = "../VKshaders/cubeShadowMapShaders/vertCubeShadowMap.spv";
        const char* fragShaderCubeShadowMap = "../VKshaders/cubeShadowMapShaders/fragCubeShadowMap.spv";

		const char* vertShaderHUD = "../VKshaders/hudShaders/hud_vert.spv";
		const char* fragShaderHUD = "../VKshaders/hudShaders/hud_frag.spv";

		const char* vertShaderFont = "../VKshaders/fontShaders/font_vert.spv";
		const char* fragShaderFont = "../VKshaders/fontShaders/font_frag.spv";

		const char* vertexShaderHudScreen = "../VKshaders/hud_screen_shaders/vert_hud_screen.spv";
		const char* fragmentShaderHudScreen = "../VKshaders/hud_screen_shaders/frag_hud_screen.spv";

		const char* vertexShaderUI = "../VKshaders/ui_shaders/vert_ui.spv";
		const char* fragmentShaderUI = "../VKshaders/ui_shaders/frag_ui.spv";

		const char* vertexShaderIconsUI = "../VKshaders/ui_icons_shaders/vert_ui_icons.spv";
		const char* fragmentShaderIconsUI = "../VKshaders/ui_icons_shaders/frag_ui_icons.spv";

		const char* virtualTexturesVertexShader = "../VKshaders/virtualTextures/virtualTexturesVert.spv";
		const char* virtualTexturesFragmentShader = "../VKshaders/virtualTextures/virtualTexturesFrag.spv";
		
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		GLVM::core::WindowWaylandVulkan* Window;
#endif
		
#ifdef VK_USE_PLATFORM_XCB_KHR
		GLVM::core::WindowXCBVulkan* Window = nullptr;
#endif
		
#ifdef VK_USE_PLATFORM_XLIB_KHR
        GLVM::core::WindowXVulkan* Window;
#endif
    
#ifdef VK_USE_PLATFORM_WIN32_KHR
        GLVM::core::WindowWinVulkan Window;
#endif
        
        CVulkanRenderer();
        ~CVulkanRenderer();

        void createTextureImage();
        void recreateSwapChain();
        void draw();
        void loadWavefrontObj();
		void EnlargeFrameAccumulator(float value);
        void SetMeshData(std::vector<const char*> _pathsArray, core::vector<const char*> pathsGLTF);
        void SetViewMatrix(mat4 _viewMatrix);
        void SetProjectionMatrix(mat4 _projectionMatrix);
		void SetViewMatrix(ecs::components::transform& _Player, ecs::components::beholder& cameraComponent);
		void SetProjectionMatrix();
		void initializeGameLevelVertices();
        void run();
    
    private:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
		mat4 viewMatrix;
		mat4 projectionMatrix;

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		VkWaylandSurfaceCreateInfoKHR createWaylandSurfaceInfo;
#endif
		
#ifdef VK_USE_PLATFORM_XLIB_KHR
        VkXlibSurfaceCreateInfoKHR createXlibSurfaceInfo;
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
        VkXcbSurfaceCreateInfoKHR createXcbSurfaceInfo;
#endif		
		
#ifdef VK_USE_PLATFORM_WIN32_KHR
        VkWin32SurfaceCreateInfoKHR createWin32SurfaceInfo;
#endif
    
        VkSurfaceKHR surface = VK_NULL_HANDLE;

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

		Pipeline mainRenderScenePipeline;
		Pipeline hudPipeline;
		VkRenderPass hudRenderPass;
		std::vector<VkDescriptorSet> hudDescriptorSets;
		VkBuffer hudUniformBuffer;
		VkDeviceMemory hudUniformBuffersMemory;
		Pipeline directionalLightPipeline;
		Pipeline spotLightPipeline;
		Pipeline pointLightPipeline;
		Pipeline fontPipeline;
		VkRenderPass fontRenderPass;
		std::vector<VkDescriptorSet> fontDescriptorSets;
		std::vector<VkDescriptorSet> fontDescriptorUboSets;
		VkBuffer fontUniformBuffer;
		VkDeviceMemory fontUniformBuffersMemory;
		Pipeline hudScreenPipeline;
		VkRenderPass hudScreenRenderPass;
		std::vector<VkDescriptorSet> hudScreenDescriptorSets;
		VkBuffer hudScreenUniformBuffer;
		VkDeviceMemory hudScreenUniformBuffersMemory;
		Pipeline uiPipeline;
		VkRenderPass uiRenderPass;
		std::vector<VkDescriptorSet> uiDescriptorSets;
		std::vector<VkDescriptorSet> uiSamplerDescriptorSets;
		VkBuffer uiUniformBuffer;
		VkDeviceMemory uiUniformBuffersMemory;
		Pipeline uiIconsPipeline;
		VkRenderPass uiIconsRenderPass;
		std::vector<VkDescriptorSet> uiIconsDescriptorSets;
		std::vector<VkDescriptorSet> uiIconsSamplerDescriptorSets;
		VkBuffer uiIconsUniformBuffer;
		VkDeviceMemory uiIconsUniformBuffersMemory;
		Pipeline virtualTexturesPipeline;
		VkRenderPass virtualTexturesRenderPass;
		std::vector<VkDescriptorSet> virtualTexturesUBODesctiptorSets;
		std::vector<VkDescriptorSet> virtualTexturesSamplersDesctiptorSets;
		VkBuffer virtualTexturesUniformBuffer;
		VkDeviceMemory virtualTexturesUniformBufferMemory;
		
        VkCommandPool directionalLightCommandPool;
		VkCommandPool spotLightCommandPool;
		VkCommandPool pointLightCommandPool;
		VkCommandPool fontCommandPool;
		VkCommandPool hudCommandPool;
		VkCommandPool hudScreenCommandPool;
		VkCommandPool uiCommandPool;
		VkCommandPool uiIconsCommandPool;
		VkCommandPool mainRenderCommandPool;
		VkCommandPool virtualTexturesCommandPool;

		/// Main pipeline depth.
		VkImage     mainDepthPipelineImage;
		VkDeviceMemory mainDepthPipelineImageMemory;
        VkImageView mainDepthImageView;

		/// Depth varialbes for shadow map.
		unsigned int	directionalLightNumber = 0;
		std::vector<VkFramebuffer> directionalLightShadowMapFrameBuffers;
		VkRenderPass directionalLightShadowMapRenderPass;
		std::vector<VkDescriptorSet> shadowMapDirectionalLightDescriptorSets;
		VkBuffer shadowMapDirectionalLightModelMatrixUniformBuffer;
		VkDeviceMemory shadowMapDirectionalLightModelMatrixUniformBuffersMemory;

		/*
		===================================
		FOR TEST ONLY!!!
		===================================
		*/
		mat4 dirLightSpaceMatrix[DIRECTIONAL_LIGHTS_NUMBER];
		mat4 spotLightSpaceMatrix[SPOT_LIGHTS_NUMBER];
		
		unsigned int	pointLightNumber	   = 0;
		std::vector<std::vector<VkFramebuffer>> pointLightShadowMapFrameBuffers;
		VkRenderPass pointLightShadowMapRenderPass;
		std::vector<VkDescriptorSet> shadowMapPointLightDescriptorSets;
		VkBuffer shadowMapPointLightModelMatrixUniformBuffer;
		VkDeviceMemory shadowMapPointLightModelMatrixUniformBuffersMemory;

		unsigned int	spotLightNumber		   = 0;
		std::vector<VkFramebuffer> spotLightShadowMapFrameBuffers;
		VkRenderPass spotLightShadowMapRenderPass;
		std::vector<VkDescriptorSet> shadowMapSpotLightDescriptorSets;
		VkBuffer shadowMapSpotLightModelMatrixUniformBuffer;
		VkDeviceMemory shadowMapSpotLightModelMatrixUniformBuffersMemory;

		std::vector<VK_Image> textureImages;
		VkSampler textureSampler;

        std::vector<VkBuffer> vertexBufferContainer;
        std::vector<VkDeviceMemory> vertexBufferMemoryContainer;
        std::vector<VkBuffer> indexBufferContainer;
        std::vector<VkDeviceMemory> indexBufferMemoryContaner;
		uint32_t wavefrontObjCounter = 0;
		uint32_t gltfCounter         = 0;

		std::vector<unsigned int> fontIndicesContainer;
        std::vector<VkBuffer> fontVertexBufferContainer;
        std::vector<VkDeviceMemory> fontVertexBufferMemoryContainer;
        std::vector<VkBuffer> fontIndexBufferContainer;
        std::vector<VkDeviceMemory> fontIndexBufferMemoryContaner;
		
        VkBuffer modelMatrixUniformBuffer;
        VkDeviceMemory modelMatrixUniformBuffersMemory;
        VkBuffer lightDataUniformBuffer;
        VkDeviceMemory lightDataUniformBuffersMemory;

		VkDescriptorImageInfo directionalLightsImageInfo[DIRECTIONAL_LIGHTS_NUMBER];
		VkDescriptorImageInfo pointLightsImageInfo[POINT_LIGHTS_NUMBER];
		VkDescriptorImageInfo spotLightsImageInfo[SPOT_LIGHTS_NUMBER];
		
        VkDescriptorPool descriptorPool;
		const unsigned int matrixUboDescriptorsNumber = 500;
		const unsigned int hudUboDescriptorNumber = 500;
		const unsigned int fontUboDescriptorNumber = 128;
		const unsigned int hudScreenUboDescriptorNumber = 32;
		const unsigned int uiUboDescriptorsNumber = 64;
		const unsigned int uiIconsDescriptorsNumber = 64;
		const unsigned int virtualTexturesDescriptorsNumber= 64;
//		unsigned int viewPositionUboDescriptorsNumber = 0;
		const unsigned int directionalLightUboDescriptorsNumber = matrixUboDescriptorsNumber * 4;        ///< 4 - maximum number of directional lights
		const unsigned int pointLightUboDescriptorsNumber = matrixUboDescriptorsNumber * 32 * 6;         ///< 32 - maximum number of point lights, 6 - number of layers for cube shadow map
		const unsigned int spotLightUboDescriptorsNumber = matrixUboDescriptorsNumber * 8;               ///< 8 - maximum number of spot lights
		u32 lightDataSize;                                                        ///< Var for choose correct number of ds from dir, spot, point light and beholder number
        std::vector<VkDescriptorSet> matrixUboDescriptorSets;
		std::vector<VkDescriptorSet> lightDataUboDescriptorSets;
		std::vector<VkDescriptorSet> diffuseSamplerDescriptorSets;
		std::vector<VkDescriptorSet> specularSamplerDescriptorSets;

        std::vector<VkCommandBuffer> directionalLightCommandBuffers;
		std::vector<VkCommandBuffer> spotLightCommandBuffers;
		std::vector<VkCommandBuffer> pointLightCommandBuffers;
		std::vector<VkCommandBuffer> fontCommandBuffers;
		std::vector<VkCommandBuffer> hudCommandBuffers;
		std::vector<VkCommandBuffer> mainRenderCommandBuffers;
		std::vector<VkCommandBuffer> virtualTexturesCommandBuffers;

		/// Main render pipe line sync objects
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

		/// Hud render pipe line sync objects
        std::vector<VkSemaphore> hudImageAvailableSemaphores;
        std::vector<VkSemaphore> hudRenderFinishedSemaphores;
        std::vector<VkFence> hudInFlightFences;

		/// Font render pipe line sync objects
        std::vector<VkSemaphore> fontImageAvailableSemaphores;
        std::vector<VkSemaphore> fontRenderFinishedSemaphores;
        std::vector<VkFence> fontInFlightFences;
		
		/// Directional light shadow map sync objects
        std::vector<VkSemaphore> directionalLightShadowMapImageAvailableSemaphores;
        std::vector<VkSemaphore> directionalLightShadowMapRenderFinishedSemaphores;
        std::vector<VkFence> directionalLightShadowMapInFlightFences;

		/// Spot light shadow map sync objects
        std::vector<VkSemaphore> spotLightShadowMapImageAvailableSemaphores;
        std::vector<VkSemaphore> spotLightShadowMapRenderFinishedSemaphores;
        std::vector<VkFence> spotLightShadowMapInFlightFences;

		/// Point light shadow map sync objects
        std::vector<VkSemaphore> pointLightShadowMapImageAvailableSemaphores;
        std::vector<VkSemaphore> pointLightShadowMapRenderFinishedSemaphores;
        std::vector<VkFence> pointLightShadowMapInFlightFences;

		/// Virtual textures pipeline sync objects
        std::vector<VkSemaphore> virtualTexturesImageAvailableSemaphores;
        std::vector<VkSemaphore> virtualTexturesRenderFinishedSemaphores;
        std::vector<VkFence> virtualTexturesInFlightFences;
		
        uint32_t currentFrame = 0;
		uint32_t fontCurrentFrame = 0;
		uint32_t hudCurrentFrame = 0;
		uint32_t directionalLightCurrentFrame = 0;
		uint32_t spotLightCurrentFrame = 0;
		uint32_t pointLightCurrentFrame = 0;

		std::mutex mutex0; 
		std::mutex mutex1;
		std::mutex mutex2;
		std::mutex shadowMapPassesMutex;
		
        bool framebufferResized = false;

        void initWindow();
		void initializeFontData();
		void initializeGLTF();
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
        void createMainRenderPass();
		void createFontRenderPass();
		void createHudRenderPass();
		void createHudScreenRenderPass();
		void createRenderPass_UI();
		void createRenderPassIcons_UI();
		void createDirectionalLightShadowMapRenderPass();
		void createSpotLightShadowMapRenderPass();
		void createPointLightShadowMapRenderPass();
		void createVirtualTextureRenderPass();
        void createDescriptorSetLayout(core::vector<Descriptor>& descriptors);
        void createGraphicsPipeline(Pipeline& pipeline, VkRenderPass& renderPass, VkPolygonMode polygonMode);
        void createRenderPassFramebuffers(std::vector<VkImageView>& attachments, VkRenderPass& renderPass_,
										  VkFramebuffer& swapChainFramebuffer, uint32_t width,
										  uint32_t height);
		void createFramebuffers();
        void createCommandPool(VkCommandPool& commandPool);
        void createDepthResources();
		void createDirectionalLightShadowMapDepthResources();
		void createSpotLightShadowMapDepthResources();
		void createPointLightShadowMapDepthResources();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();
        bool hasStencilComponent(VkFormat format);
        void createTextureImageView();
        void createTextureSampler();
        VkImageView createImageView(VK_Image image, uint32_t baseArrayLayers, uint32_t layerCount);
        void createImage(VK_Image& image);
        void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
		void transitionShadowMapImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer& buffer, VkImage image, uint32_t width, uint32_t height);
        void createVertexBuffer(VkBuffer& _vertexBuffer, VkDeviceMemory& _vertexBufferMemory, core::vector<Vertex>& _vertices);
        void createIndexBuffer(VkBuffer& _indexBuffer, VkDeviceMemory& _indexBufferMemory, const std::vector<uint32_t>& _indices);
        void createMainRenderUniformBuffers();
        void createMainRenderDescriptorPool();
		void allocateDescriptorSets( std::vector<VkDescriptorSet>& descriptorSets, const Pipeline& pipeline,
									 unsigned int desriptorID, unsigned int descriptorSetsNumber);
		void updateDescriptorSetsUBO( VkBuffer ubo, const VkDeviceSize& uboStructSize, const unsigned int& uboDescriptorsNumber,
									  int uboBinding, std::vector<VkDescriptorSet> uboDescriptorSets);
		void updateDescriptorSetsCombinedImageSampler( std::vector<VK_Image>& textureImages, const unsigned int& descriptorSetsNumber,
													   const core::vector<unsigned int> bindings, std::vector<VkDescriptorSet>& descriptorSets,
													   const unsigned int descriptorCount );
		void createDescriptorImageInfo( const unsigned int descriptorNumber, VkImageLayout imageLayout,
										std::vector<VK_Image>& textureImages, const unsigned int imageViewIndex,
										VkDescriptorImageInfo descriptorImageInfos[]);
		void updateDescriptorSets( std::vector<VkDescriptorSet>& descriptorSets, const Pipeline& pipeline, DescriptorsTypes descriptorType );
		void createDirectionalLightShadowMapDescriptorSets();
		void createSpotLightShadowMapDescriptorSets();
		void createPointLightShadowMapDescriptorSets();
		void createHudDescriptorSets();
		void createHudScreenDescriptorSets();
		void createDescriptorSets_UI();
		void createDescriptorSetsIcons_UI();
		void createFontRenderDescriptorSets();
		void createVirtualTexturesDescriptorSets();
        void createMainRenderDescriptorSets();
		void updateSamplersDescriptroSets(uint32_t diffuse_id, uint32_t specular_id );
		void updateDirectionalLightShadowMapDescriptorSets();
		void updateSpotLightShadowMapDescriptorSets();
		void updatePointLightShadowMapDescriptorSets();
		void updateHudDescriptorSets();
		void updateFontRenderDescriptorSets();
		void updateVirtualTexturesDescriptorSets();
		void updateDescriptorSets();
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        VkCommandBuffer beginSingleTimeCommands(VkCommandPool& commandPool);
        void endSingleTimeCommands(VkCommandPool& commandPool, VkCommandBuffer& commandBuffer);
        void copyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void createCommandBuffers(VkCommandPool& commandPool, std::vector<VkCommandBuffer>& commandBuffers);
		void updateHudUBO(uint32_t offset, ecs::components::transform* entityOwnHudTransform,
						  ecs::components::health* entityOwnHudHealth, bool isHudExists, float highestY);
		void updateHudScreenUBO(uint32_t offset, ecs::components::transform* cursorTransform);
		void updateUBO_UI(float x_slot_offset, float y_slot_offset, uint32_t offset,
						  ecs::components::transform* inventorySlotTransform, unsigned int inventorySlotEntity);
		void updateUBO_IconsUI(uint32_t offset, ecs::components::transform* itemTransfromComponent,
							   ecs::components::collider* itemColliderComponent, ecs::components::item* itemComponent);
		void hudRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
		void uiRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
		void uiIconsRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
		void hudScreenRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
		void fontRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
        void recordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
        void createSyncObjects(std::vector<VkSemaphore>& imageAvailableSemaphores,
							   std::vector<VkSemaphore>& renderFinishedSemaphores,
							   std::vector<VkFence>& inFlightFences);
		void updateDirectionalLightSpaceMatrixShadowMapUBO(ecs::components::directionalLight* directionalLightComponent, uint32_t currentLight);
		void updateDirectionalLightShadowMapMatrixUBO(uint32_t currentImage, ecs::components::transform* _transformComponent, uint32_t currentLight, u32 meshID);
		void updateSpotLightSpaceMatrixShadowMapUBO(ecs::components::spotLight* spotLightComponent,
																		 uint32_t currentLight);
		void updateSpotLightShadowMapMatrixUBO(uint32_t currentImage, ecs::components::transform* _transformComponent, uint32_t currentLight, u32 meshID);
		void updatePointLightShadowMapMatrixUBO(uint32_t currentImage, ecs::components::transform* _transformComponent, ecs::components::pointLight* pointLightComponent, uint32_t layer, unsigned int meshID);
        void updateMatrixUniformBuffer(uint32_t offset, ecs::components::transform* _transformComponent,
									   unsigned int meshID, ecs::components::material* materialComponent);
		void updateViewPositionUniformBuffer(uint32_t currentImage, ecs::components::transform* transformComponent);
		void hudDrawFrame();
		void fontDrawFrame();
        void mainRenderDrawFrame();
		void directionalLightShadowMapDrawFrame();
		void spotLightShadowMapDrawFrame();
		void pointLightShadowMapDrawFrame();
		void directionalLightRecordCoomandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
		void spotLightRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
		void pointLightRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
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
		[[nodiscard]] mat4* updateAnimationFrames(ecs::components::transform* _transformComponent, unsigned int meshID);
		mat4 computeModelMatrix(ecs::components::transform* _transformComponent);
		void setImageDebugObjectName(VK_Image image);
		void setDebugObjectNames();
		void clearPipeline( Pipeline& pipeline );
    };

};

#endif
