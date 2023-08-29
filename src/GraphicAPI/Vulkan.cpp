#include "ComponentManager.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Texture.hpp"
#include "Vector.hpp"
#include "WavefrontObjParser.hpp"
#include <cstddef>
#include <cstdlib>
#include <vulkan/vulkan_core.h>
//#include <vulkan/vulkan_core.h>

namespace GLVM::core
{    
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

	VkResult SetDebugObjectName(VkDevice device, const VkDebugUtilsObjectNameInfoEXT* objectNameInfo) {
		auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
		if (func != nullptr) {
			return func(device, objectNameInfo);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    CVulkanRenderer::CVulkanRenderer() {
    }
    
    CVulkanRenderer::~CVulkanRenderer() {
        cleanup();
    }
    
    void CVulkanRenderer::draw() {
		namespace cm = GLVM::ecs::components;
		
		ecs::ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::beholder>();
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
		for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
			Entity currentEntity                = linkedEntities[i];
			cm::beholder* beholderComponent     = componentManager->GetComponent<cm::beholder>(currentEntity);
			cm::transform* transformComponent   = componentManager->GetComponent<cm::transform>(currentEntity);
			SetViewMatrix(*transformComponent, *beholderComponent);
		}
		
		SetProjectionMatrix();
//		drawShadowMapFrame();
        drawFrame();
        vkDeviceWaitIdle(device);
    }

    void CVulkanRenderer::loadWavefrontObj() {
        for (unsigned int m = 0; m < pathsArray_.size(); ++m) {
            CWaveFrontObjParser parser;
            CWaveFrontObjParser* wavefrontObjParser = &parser;
            
            wavefrontObjParser->ReadFile(pathsArray_[m]);
            wavefrontObjParser->ParseFile();

            aIndices_.emplace_back();
            aVertices_.emplace_back();
            
            unsigned int vertexIndex  = 0;
            unsigned int textureIndex = 0;
			unsigned int normalIndex  = 0;
            unsigned int faceVerticesSize = wavefrontObjParser->getFaces().GetSize();

            for (unsigned int i = 0; i < faceVerticesSize; ++i)
                for (int j = 0; j < 3; ++j) {
                    vertexIndex     = wavefrontObjParser->getFaces()[i][0][j] - 1;
					aIndices_[m].push_back(i * 3 + j);
                    SVertex vertex  = wavefrontObjParser->getCoordinateVertices()[vertexIndex];
                    textureIndex    = wavefrontObjParser->getFaces()[i][1][j] - 1;
                    SVertex texture = wavefrontObjParser->getTextureVertices()[textureIndex];
					normalIndex     = wavefrontObjParser->getFaces()[i][2][j] - 1;
					SVertex normal  = wavefrontObjParser->getNormals()[normalIndex];
                    aVertices_[m].push_back({{vertex[0], vertex[1], vertex[2]},
											 {normal[0], normal[1], normal[2]},
											 {texture[0], texture[1]}});
                }
			
            vertexBufferContainer.emplace_back();
            vertexBufferMemoryContainer.emplace_back();
            createVertexBuffer(vertexBufferContainer[m], vertexBufferMemoryContainer[m], aVertices_[m]);

            indexBufferContainer.emplace_back();
            indexBufferMemoryContaner.emplace_back();
            createIndexBuffer(indexBufferContainer[m], indexBufferMemoryContaner[m], aIndices_[m]);
        }
    }

	void CVulkanRenderer::LoadGLTF() {
	}

	void CVulkanRenderer::EnlargeFrameAccumulator([[maybe_unused]] float value) {
	}
	
    void CVulkanRenderer::SetViewMatrix(mat4 _viewMatrix) {
        viewMatrix = _viewMatrix; // 
    }
    
    void CVulkanRenderer::SetProjectionMatrix(mat4 _projectionMatrix) {
        projectionMatrix = _projectionMatrix;
    }

    void CVulkanRenderer::SetViewMatrix(ecs::components::transform& _Player, ecs::components::beholder& cameraComponent)
    {
		Matrix<float, 4> viewMatrix_(1.0f);
        const float kSensitivity = 0.1f;

        fYaw = g_eEvent.mousePointerPosition.offset_X;
        fPitch = g_eEvent.mousePointerPosition.offset_Y;
        fYaw *= kSensitivity;
        fPitch *= kSensitivity;

        g_eEvent.mousePointerPosition.pitch = fPitch;
        g_eEvent.mousePointerPosition.yaw = fYaw;
        
        if(fPitch > 89.0f)
            fPitch = 89.0f;
        if(fPitch < -89.0f)
            fPitch = -89.0f;

		vec3 forward;
		float sinPitch = std::sin(Radians(fPitch / 2));
		float cosPitch = std::cos(Radians(fPitch / 2));
		float sinYaw = std::sin(Radians(-fYaw / 2));
		float cosYaw = std::cos(Radians(-fYaw / 2));
		
		Quaternion pitchQuat;
		Quaternion yawQuat;
		pitchQuat.w = cosPitch;
		pitchQuat.x = sinPitch;
		pitchQuat.y = 0.0f;
		pitchQuat.z = 0.0f;

		yawQuat.w = cosYaw;
		yawQuat.x = 0.0f;
		yawQuat.y = sinYaw;
		yawQuat.z = 0.0f;
		
		Quaternion result;
		result = multiplyQuaternion(yawQuat, pitchQuat);

		result = multiplyQuaternion(multiplyQuaternion(result, Quaternion{ .w = 0.0f, .x = 0.0f,
					.y = 0.0f, .z = -1.0f }), inverseQuaternion(result));

		forward[0] = result.x;
		forward[1] = result.y;
		forward[2] = result.z;
        cameraComponent.forward = Normalize(forward);

        viewMatrix_ = LookAtMain(_Player.tPosition,
								_Player.tPosition + cameraComponent.forward,
								cameraComponent.up);

		viewMatrix = viewMatrix_;
    }

    void CVulkanRenderer::SetProjectionMatrix()
	{
		mat4 tProjection_Matrix = Perspective(Radians(90.0f), (float)1920 / (float)1080, 1.0f, 100.0f);
		projectionMatrix = tProjection_Matrix;
	}
	
    void CVulkanRenderer::createTextureImage() {
        int texWidth, texHeight;

        for(unsigned int i = 0; i < initializeTextureData_.size(); ++i)
        {
            VkDeviceSize imageSize = initializeTextureData_[i].dat_length_;
            const unsigned char* pixels = initializeTextureData_[i].u_iData_;
            texWidth = initializeTextureData_[i].iWidth_;
            texHeight = initializeTextureData_[i].iHeight_;

            if (!pixels) {
                throw std::runtime_error("failed to load texture image!");
            }

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(device, stagingBufferMemory);
            createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImages[i], textureImageMemories[i]);

            transitionImageLayout(textureImages[i], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            copyBufferToImage(stagingBuffer, textureImages[i], static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
            transitionImageLayout(textureImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }
    }

    void CVulkanRenderer::recreateSwapChain() {
        // int width = 0, height = 0;
        // glfwGetFramebufferSize(window, &width, &height);
        // while (width == 0 || height == 0) {
        //     glfwGetFramebufferSize(window, &width, &height);
        //     glfwWaitEvents();
        // }

        vkDeviceWaitIdle(device);

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createDepthResources();
		createShadowMapDepthResources();
        createFramebuffers();
		updateDirectionalLightShadowMapDescriptorSets();
		updateDescriptorSets();
    }
    
    void CVulkanRenderer::SetTextureData(std::vector<ecs::Texture>& _texture_data) {
        texture_load_data_ = _texture_data;

        // textureImages.resize(texture_load_data_.size());
        // textureImageMemories.resize(texture_load_data_.size());

        // textureImageViews.resize(texture_load_data_.size());
        // textureSamplers.resize(texture_load_data_.size());
    }

    void CVulkanRenderer::SetMeshData(std::vector<const char*> _pathsArray, core::vector<const char*> pathsGLTF) {
        for (unsigned int i = 0; i < _pathsArray.size(); ++i)
            pathsArray_.push_back(_pathsArray[i]);

		for (unsigned int i = 0; i < pathsGLTF.GetSize(); ++i)
			pathsGLTF_.Push(pathsGLTF[i]);
    }
    
    void CVulkanRenderer::run() {
        GLVM::core::MeshManager*   meshManager = GLVM::core::MeshManager::GetInstance();

		SetMeshData(meshManager->pathsArray_, meshManager->pathsGLTF_);

		unsigned int mainTexturesQuantity = initializeTextureData_.size();

        textureImages.resize(mainTexturesQuantity);
        textureImageMemories.resize(mainTexturesQuantity);

        textureImageViews.resize(mainTexturesQuantity);
        textureSamplers.resize(mainTexturesQuantity);

		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager   = ecs::ComponentManager::GetInstance();
		core::vector<Entity> directionalLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																									  cm::directionalLight,
																									  cm::vertex>();
		
		directionalLightShadowMapMatrixUboDescriptorsNumber = directionalLightLinkedEntities.GetSize();

		
		directionalLightShadowMapTextureSamplers.resize(directionalLightShadowMapMatrixUboDescriptorsNumber);

		directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		// directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		// directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		// directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		// directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		// directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		// directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		// directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		// directionalLightPipeline.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		directionalLightPipeline.vertShader = vertShaderMain_;
		directionalLightPipeline.fragShader = fragShaderMain_;
		
		directionalLightPipeline.bindingDescription = Vertex::getBindingDescription();
		directionalLightPipeline.attributeDescriptions = Vertex::getAttributeDescriptions();
		
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		mainRenderScenePipeline.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		mainRenderScenePipeline.vertShader = vertShaderMain_;
		mainRenderScenePipeline.fragShader = fragShaderMain_;

		mainRenderScenePipeline.bindingDescription = Vertex::getBindingDescription();
		mainRenderScenePipeline.attributeDescriptions = Vertex::getAttributeDescriptions();
		
        initWindow();
        initVulkan();
//        mainLoop();
//        cleanup();
    }
    
    void CVulkanRenderer::initWindow() {
#ifdef VK_USE_PLATFORM_XLIB_KHR
        createXlibSurfaceInfo.dpy = Window.GetDisplay();
        createXlibSurfaceInfo.window = Window.GetWindow();

        createXlibSurfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        createXlibSurfaceInfo.pNext = nullptr;
        createXlibSurfaceInfo.flags = 0;
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
        createWin32SurfaceInfo.hwnd = Window.GetModernWindowHWND();
        
        createWin32SurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createWin32SurfaceInfo.pNext = nullptr;
        createWin32SurfaceInfo.flags = 0;
#endif

		
        ///< glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

// static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
//     auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
//     app->framebufferResized = true;
// }

    void CVulkanRenderer::initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
		createShadowMapRenderPass();
        createDescriptorSetLayout(directionalLightPipeline.descriptors);
        createDescriptorSetLayout(mainRenderScenePipeline.descriptors);
//        createDescriptorSetLayout(descriptorSetLayoutHUD);
        createGraphicsPipeline(directionalLightPipeline, directionalLightShadowMapRenderPass);
		createGraphicsPipeline(mainRenderScenePipeline, renderPass);
//        createGraphicsPipeline(graphicsPipelineHUD, pipelineLayoutHUD, descriptorSetLayoutHUD, vertShaderHUD_, fragShaderHUD_);
//		createShadowMapCommandPool();             ///< DELETE!!!!!!!!!
        createCommandPool();
        createDepthResources();
		createShadowMapDepthResources();
        createFramebuffers();
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
		createShadowMapTextureSamplers();
        // createVertexBuffer(vertexBuffer, vertexBufferMemory, verticesContainer_);
        // createIndexBuffer(indexBuffer, indexBufferMemory, indicesContainer_);
        loadWavefrontObj();                                                           ///< Both second functions inside this function
//		createVertexBuffer(hudVertexBuffer, hudVertexBufferMemory, hudVertices);
//		createIndexBuffer(hudIndexBuffer, hudIndexBufferMemory, hudIndices);
		createDirectionalLightShadowMapUniformBuffers();
		createShadowMapDescriptorPool();
		createDirectionalLightShadowMapDescriptorSets();
        createMainRenderUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
		setDebugObjectNames();
//		createShadowMapCommandBuffers();           ///< DELETE!!!!!!!!!!!!
        createCommandBuffers();
		createShadowMapSyncObjects();
        createSyncObjects();
    }

    void CVulkanRenderer::cleanupSwapChain() {
        vkDestroyImageView(device, depthImageView, nullptr);
        vkDestroyImage(device, depthImage, nullptr);
        vkFreeMemory(device, depthImageMemory, nullptr);

        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void CVulkanRenderer::cleanup() {
        cleanupSwapChain();

        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(device, modelMatrixUniformBuffers[i], nullptr);
            vkFreeMemory(device, modelMatrixUniformBuffersMemory[i], nullptr);
			// vkDestroyBuffer(device, viewPositionUniformBuffers[i], nullptr);
            // vkFreeMemory(device, viewPositionUniformBuffersMemory[i], nullptr);
			// vkDestroyBuffer(device, materialUniformBuffers[i], nullptr);
            // vkFreeMemory(device, materialUniformBuffersMemory[i], nullptr);
			// vkDestroyBuffer(device, directionalLightsUniformBuffers[i], nullptr);
            // vkFreeMemory(device, directionalLightsUniformBuffersMemory[i], nullptr);
			// vkDestroyBuffer(device, pointLightsUniformBuffers[i], nullptr);
            // vkFreeMemory(device, pointLightsUniformBuffersMemory[i], nullptr);
			vkDestroyBuffer(device, spotLightsUniformBuffers[i], nullptr);
            vkFreeMemory(device, spotLightsUniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(device, descriptorPool, nullptr);

        for(unsigned int i = 0; i < initializeTextureData_.size(); ++i)
        {
            vkDestroySampler(device, textureSamplers[i], nullptr);
            vkDestroyImageView(device, textureImageViews[i], nullptr);
        }

        for (unsigned int i = 0; i < initializeTextureData_.size(); ++i)
        {
            vkDestroyImage(device, textureImages[i], nullptr);
            vkFreeMemory(device, textureImageMemories[i], nullptr);
        }

		for ( unsigned int i = 0; i < directionalLightPipeline.descriptors.GetSize(); ++i ) 
			vkDestroyDescriptorSetLayout(device, directionalLightPipeline.descriptors[i].setLayout, nullptr);

		for ( unsigned int i = 0; i < mainRenderScenePipeline.descriptors.GetSize(); ++i ) 
			vkDestroyDescriptorSetLayout(device, mainRenderScenePipeline.descriptors[i].setLayout, nullptr);
		
        for (size_t i = 0; i < vertexBufferContainer.size(); ++i) {
            vkDestroyBuffer(device, indexBufferContainer[i], nullptr);
            vkFreeMemory(device, indexBufferMemoryContaner[i], nullptr);

            vkDestroyBuffer(device, vertexBufferContainer[i], nullptr);
            vkFreeMemory(device, vertexBufferMemoryContainer[i], nullptr);
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device, commandPool, nullptr);

        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
//        Window.Close();
    }

    void CVulkanRenderer::createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Grey Lane Vertex Machine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void CVulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void CVulkanRenderer::setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void CVulkanRenderer::createSurface() {
#ifdef VK_USE_PLATFORM_XLIB_KHR
        if (vkCreateXlibSurfaceKHR(instance, &createXlibSurfaceInfo, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (vkCreateWin32SurfaceKHR(instance, &createWin32SurfaceInfo, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
#endif
    }

    void CVulkanRenderer::pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const VkPhysicalDevice& device : devices) {
            if (isDeviceSuitable(device)) {
				// VkPhysicalDeviceProperties properties;
				// vkGetPhysicalDeviceProperties(device, &properties);
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void CVulkanRenderer::createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

    void CVulkanRenderer::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void CVulkanRenderer::createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (uint32_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void CVulkanRenderer::createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = 0;
        dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
//        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT ;
        
        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void CVulkanRenderer::createShadowMapRenderPass() {
        VkAttachmentDescription attachmentDescription{};
        
		attachmentDescription.format = findDepthFormat();
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		/// Attachment references form subpasses
        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 0;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		/// Subpass 0: shadow map rendering
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 0;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependencies[2];
		dependencies[0].srcSubpass		= VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass		= 0;
		dependencies[0].srcStageMask	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstStageMask	= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask	= VK_ACCESS_SHADER_READ_BIT;
		dependencies[0].dstAccessMask	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass		= 0;
		dependencies[1].dstSubpass		= VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask	= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[1].dstStageMask	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask	= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask	= VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &attachmentDescription;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = dependencies;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &directionalLightShadowMapRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
	
    void CVulkanRenderer::createDescriptorSetLayout(core::vector<Descriptor>& descriptors) {
		for ( unsigned int i = 0; i < descriptors.GetSize(); ++i ) {
			VkDescriptorSetLayoutBinding modelMatrixUboLayout{};
			modelMatrixUboLayout.binding = descriptors[i].binding;
			modelMatrixUboLayout.descriptorCount = 1;
			modelMatrixUboLayout.descriptorType = descriptors[i].type;
			modelMatrixUboLayout.pImmutableSamplers = nullptr;
			modelMatrixUboLayout.stageFlags = descriptors[i].shaderStageFlag;

			std::array<VkDescriptorSetLayoutBinding, 1> bindings = {modelMatrixUboLayout};
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.flags = 0;
			layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			layoutInfo.pBindings = bindings.data();

			if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptors[i].setLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}
    }

    void CVulkanRenderer::createGraphicsPipeline(Pipeline& pipeline, VkRenderPass& renderPass) {
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
		if (pipeline.vertShader != nullptr) {
			std::vector<char> vertShaderCode = readFile(pipeline.vertShader);
			vertShaderModule = createShaderModule(vertShaderCode);
		
			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";

			shaderStages.push_back(vertShaderStageInfo);
		}

		if (pipeline.fragShader != nullptr) {
			std::vector<char> fragShaderCode = readFile(pipeline.fragShader);
			fragShaderModule = createShaderModule(fragShaderCode);

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			shaderStages.push_back(fragShaderStageInfo);
		}

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        // VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
		// std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(pipeline.attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &pipeline.bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = pipeline.attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
//        rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//        rasterizer.polygonMode = VK_POLYGON_MODE_POINT;
        rasterizer.lineWidth = 1.0f;
//        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.cullMode = 0;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

		unsigned int descriptorLayoutsNumber = pipeline.descriptors.GetSize();
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		for (unsigned int i = 0; i < descriptorLayoutsNumber; ++i)
			descriptorSetLayouts.push_back(pipeline.descriptors[i].setLayout);
			
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = descriptorLayoutsNumber;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        // VkPipelineLayoutCreateInfo pipelineLayoutInfo2{};
        // pipelineLayoutInfo2.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        // pipelineLayoutInfo2.setLayoutCount = 1;
        // pipelineLayoutInfo2.pSetLayouts = &_descriptorSetLayout2;

        // if (vkCreatePipelineLayout(device, &pipelineLayoutInfo2, nullptr, &_pipelineLayout) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to create pipeline layout!");
        // }
		
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipeline.pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }

    void CVulkanRenderer::createFramebuffers() {
		/// Main renderer frame buffers initialization
		swapChainFramebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
			std::vector<VkImageView> mainRenderAttachments;
			mainRenderAttachments.push_back(swapChainImageViews[i]);
			mainRenderAttachments.push_back(depthImageView);

			createRenderPassFramebuffers(mainRenderAttachments, renderPass, swapChainFramebuffers[i]);
		};

		/// Directional lights shdow map renderer frame buffers initialization
		directionalLightShadowMapFrameBuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
			std::vector<VkImageView> directionalLightsRenderAttachments;
			directionalLightsRenderAttachments.push_back(directionalLightShadowMapDepthImageViews[0]);

			createRenderPassFramebuffers(directionalLightsRenderAttachments, directionalLightShadowMapRenderPass, directionalLightShadowMapFrameBuffers[i]);
		};
    }

    void CVulkanRenderer::createRenderPassFramebuffers(std::vector<VkImageView>& attachments, VkRenderPass& renderPass_,
													   VkFramebuffer& swapChainFramebuffer) {
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass_;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;
			
            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
    }
	
	void CVulkanRenderer::createShadowMapCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &shadowMapCommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }
	
    void CVulkanRenderer::createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }

    void CVulkanRenderer::createDepthResources() {
        VkFormat depthFormat = findDepthFormat();

        createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void CVulkanRenderer::createShadowMapDepthResources() {
//        VkFormat depthFormat = findDepthFormat();

		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager   = ecs::ComponentManager::GetInstance();
		core::vector<Entity> directionalLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																									  cm::directionalLight,
																									  cm::vertex>();
		directionalLightNumber = directionalLightLinkedEntities.GetSize();
		createShadowMapData(directionalLightNumber, directionalLightShadowMapDepthImages,
						    directionalLightShadowMapDepthImageMemories, directionalLightShadowMapDepthImageViews);
		
		core::vector<Entity> pointLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																								cm::pointLight,
																								cm::vertex>();
		pointLightNumber = pointLightLinkedEntities.GetSize();
		createShadowMapData(pointLightNumber, pointLightShadowMapDepthImages,
							pointLightShadowMapDepthImageMemories, pointLightShadowMapDepthImageViews);

		core::vector<Entity> spotLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							   cm::spotLight,
																							   cm::vertex>();
		spotLightNumber = spotLightLinkedEntities.GetSize();
		createShadowMapData(spotLightNumber, spotLightShadowMapDepthImages,
							spotLightShadowMapDepthImageMemories, spotLightShadowMapDepthImageViews);
    }

	void CVulkanRenderer::createShadowMapData(unsigned int lightsNumber, core::vector<VkImage>& shadowMapDepthImages,
											  core::vector<VkDeviceMemory>& shadowMapDepthImageMemories,
											  core::vector<VkImageView>& shadowMapDepthImageViews) {
 		shadowMapDepthImages.Resize(lightsNumber);
		shadowMapDepthImageMemories.Resize(lightsNumber);
		shadowMapDepthImageViews.Resize(lightsNumber);

		for ( unsigned int i = 0; i < lightsNumber; ++i ) {		
			createImage(swapChainExtent.width, swapChainExtent.height, findDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shadowMapDepthImages[i], shadowMapDepthImageMemories[i]);
			shadowMapDepthImageViews[i] = createShadowMapImageView(shadowMapDepthImages[i], findDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);
		}
	}
	
    VkFormat CVulkanRenderer::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat CVulkanRenderer::findDepthFormat() {
        return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
            );
    }

    bool CVulkanRenderer::hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void CVulkanRenderer::createTextureImageView() {
        for(unsigned int i = 0; i < initializeTextureData_.size(); ++i)
            textureImageViews[i] = createImageView(textureImages[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void CVulkanRenderer::createTextureSampler() {
        for(unsigned int i = 0; i < initializeTextureData_.size(); ++i)
        {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);

            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_NEAREST;
            samplerInfo.minFilter = VK_FILTER_NEAREST;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

            if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSamplers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture sampler!");
            }
        }
    }

    void CVulkanRenderer::createShadowMapTextureSamplers() {
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager = ecs::ComponentManager::GetInstance();
		core::vector<Entity> directionalLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																									  cm::directionalLight,
																									  cm::vertex>();
		
        for(unsigned int i = 0; i < directionalLightLinkedEntities.GetSize(); ++i)
			createRenderPassShadowMapTextureSamplers(directionalLightShadowMapTextureSamplers[i]);
    }

	void CVulkanRenderer::createRenderPassShadowMapTextureSamplers(VkSampler& shadowMapTextureSampler) {
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkFilter shadowMapFilter = VK_FILTER_LINEAR;
			
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = shadowMapFilter;
		samplerInfo.minFilter = shadowMapFilter;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = samplerInfo.addressModeU;
		samplerInfo.addressModeW = samplerInfo.addressModeU;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;

		if (vkCreateSampler(device, &samplerInfo, nullptr, &shadowMapTextureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}
	
    VkImageView CVulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    VkImageView CVulkanRenderer::createShadowMapImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
		viewInfo.subresourceRange = {};
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }
	
    void CVulkanRenderer::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
//        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    void CVulkanRenderer::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
            );

        endSingleTimeCommands(commandBuffer);
    }

    void CVulkanRenderer::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(commandBuffer);
    }

    void CVulkanRenderer::createVertexBuffer(VkBuffer& _vertexBuffer, VkDeviceMemory& _vertexBufferMemory, const std::vector<Vertex>& _vertices) {
        VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, _vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

        copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void CVulkanRenderer::createIndexBuffer(VkBuffer& _indexBuffer, VkDeviceMemory& _indexBufferMemory, const std::vector<uint16_t>& _indices) {
        VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, _indices.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

        copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

	void CVulkanRenderer::createDirectionalLightShadowMapUniformBuffers() {
		VkDeviceSize modelMatrixBufferSize = sizeof(DirectionalLightShadowMapMatrixUBO);

		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager   = ecs::ComponentManager::GetInstance();
		core::vector<Entity> directionalLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																									  cm::directionalLight,
																									  cm::vertex>();
		
		directionalLightShadowMapMatrixUboDescriptorsNumber = directionalLightLinkedEntities.GetSize();
		
        shadowMapDirectionalLightModelMatrixUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber);
        shadowMapDirectionalLightModelMatrixUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber; i++) {
            createBuffer(modelMatrixBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						 shadowMapDirectionalLightModelMatrixUniformBuffers[i], shadowMapDirectionalLightModelMatrixUniformBuffersMemory[i]);
		}
	}
	
    void CVulkanRenderer::createMainRenderUniformBuffers() {
        VkDeviceSize modelMatrixBufferSize = sizeof(ModelMatrixUBO);
		VkDeviceSize viewPositionBufferSize = sizeof(ViewPositionUBO);
		VkDeviceSize materialBufferSize = sizeof(MaterialUBO);
        VkDeviceSize directionalLightsBufferSize = sizeof(DirectionalLightsUBO);
		VkDeviceSize pointLightsBufferSize = sizeof(PointLightsUBO);
		VkDeviceSize spotLightsBufferSize = sizeof(SpotLightsUBO);

		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager   = ecs::ComponentManager::GetInstance();
		core::vector<Entity> matrixLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							cm::material,
																							cm::vertex>();
		
		matrixUboDescriptorsNumber = matrixLinkedEntities.GetSize();
		
        modelMatrixUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber);
        modelMatrixUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber; i++) {
            createBuffer(modelMatrixBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						 modelMatrixUniformBuffers[i], modelMatrixUniformBuffersMemory[i]);
		}

		core::vector<Entity> viewPositionLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																								  cm::beholder,
																								  cm::vertex>();
		
		viewPositionUboDescriptorsNumber = viewPositionLinkedEntities.GetSize();
		
        viewPositionUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber);
        viewPositionUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber; i++) {
            createBuffer(viewPositionBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						 viewPositionUniformBuffers[i], viewPositionUniformBuffersMemory[i]);
		}

		core::vector<Entity> materialLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							  cm::material,
																							  cm::vertex>();
		
		materialUboDescriptorsNumber = materialLinkedEntities.GetSize();
		
        materialUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber);
        materialUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber; i++) {
            createBuffer(materialBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						 materialUniformBuffers[i], materialUniformBuffersMemory[i]);
		}

		core::vector<Entity> directionalLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																									  cm::directionalLight,
																									  cm::vertex>();
		
		directionalLightUboDescriptorsNumber = directionalLightLinkedEntities.GetSize();
		
        directionalLightsUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        directionalLightsUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(directionalLightsBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						 directionalLightsUniformBuffers[i], directionalLightsUniformBuffersMemory[i]);
		}

		core::vector<Entity> pointLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																								cm::pointLight,
																								cm::vertex>();
		
		pointLightUboDescriptorsNumber = pointLightLinkedEntities.GetSize();
		
        pointLightsUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        pointLightsUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(pointLightsBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						 pointLightsUniformBuffers[i], pointLightsUniformBuffersMemory[i]);
		}

		core::vector<Entity> spotLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							   cm::spotLight,
																							   cm::vertex>();
		
		spotLightUboDescriptorsNumber = spotLightLinkedEntities.GetSize();
		
        spotLightsUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        spotLightsUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(spotLightsBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						 spotLightsUniformBuffers[i], spotLightsUniformBuffersMemory[i]);
		}
    }

    void CVulkanRenderer::createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 9> poolSizes{};


        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber);
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber);
		poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[3].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[4].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[4].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[5].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[5].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[6].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[6].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size());
		poolSizes[7].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[7].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size());
		poolSizes[8].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[8].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
		unsigned int lightsTypesQuantity = 3;
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * (2 * initializeTextureData_.size() +
																		 matrixUboDescriptorsNumber + viewPositionUboDescriptorsNumber +
																		 materialUboDescriptorsNumber) +
												 lightsTypesQuantity * MAX_FRAMES_IN_FLIGHT + MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void CVulkanRenderer::createShadowMapDescriptorPool() {
        std::array<VkDescriptorPoolSize, 1> poolSizes{};

        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &shadowMapDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void CVulkanRenderer::createDirectionalLightShadowMapDescriptorSets() {
		std::vector<VkDescriptorSetLayout> matrixUboLayouts(MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber,
															directionalLightPipeline.descriptors[0].setLayout);
		VkDescriptorSetAllocateInfo matrixUboAllocInfo{};
		matrixUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		matrixUboAllocInfo.descriptorPool = shadowMapDescriptorPool;
		matrixUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT *
																	  directionalLightShadowMapMatrixUboDescriptorsNumber);
		matrixUboAllocInfo.pSetLayouts = matrixUboLayouts.data();
			
		shadowMapDirectionalLightDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber);
		if (vkAllocateDescriptorSets(device, &matrixUboAllocInfo, shadowMapDirectionalLightDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		//        int textureImageViewsIndex = 0;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = shadowMapDirectionalLightModelMatrixUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(DirectionalLightShadowMapMatrixUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = shadowMapDirectionalLightDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
	
    void CVulkanRenderer::createDescriptorSets() {
		std::vector<VkDescriptorSetLayout> matrixUboLayouts(MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber,
															mainRenderScenePipeline.descriptors[0].setLayout);
		VkDescriptorSetAllocateInfo matrixUboAllocInfo{};
		matrixUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		matrixUboAllocInfo.descriptorPool = descriptorPool;
		matrixUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber);
		matrixUboAllocInfo.pSetLayouts = matrixUboLayouts.data();
			
		matrixUboDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber);
		if (vkAllocateDescriptorSets(device, &matrixUboAllocInfo, matrixUboDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		//        int textureImageViewsIndex = 0;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = modelMatrixUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(ModelMatrixUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = matrixUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		std::vector<VkDescriptorSetLayout> viewPositionUboLayouts(MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber,
																  mainRenderScenePipeline.descriptors[1].setLayout);
		VkDescriptorSetAllocateInfo viewPositionUboAllocInfo{};
		viewPositionUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		viewPositionUboAllocInfo.descriptorPool = descriptorPool;
		viewPositionUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber);
		viewPositionUboAllocInfo.pSetLayouts = viewPositionUboLayouts.data();
			
		viewPositionUboDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber);
		if (vkAllocateDescriptorSets(device, &viewPositionUboAllocInfo, viewPositionUboDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = viewPositionUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(ViewPositionUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = viewPositionUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 1;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		std::vector<VkDescriptorSetLayout> materialUboLayouts(MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber,
															  mainRenderScenePipeline.descriptors[2].setLayout);
		VkDescriptorSetAllocateInfo materialUboAllocInfo{};
		materialUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		materialUboAllocInfo.descriptorPool = descriptorPool;
		materialUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber);
		materialUboAllocInfo.pSetLayouts = materialUboLayouts.data();
			
		materialUboDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber);
		if (vkAllocateDescriptorSets(device, &materialUboAllocInfo, materialUboDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = materialUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(MaterialUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = materialUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 2;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		std::vector<VkDescriptorSetLayout> directionalLightUboLayouts(MAX_FRAMES_IN_FLIGHT, mainRenderScenePipeline.descriptors[3].setLayout);
		VkDescriptorSetAllocateInfo directionalLightUboAllocInfo{};
		directionalLightUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		directionalLightUboAllocInfo.descriptorPool = descriptorPool;
		directionalLightUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		directionalLightUboAllocInfo.pSetLayouts = directionalLightUboLayouts.data();
			
		directionalLightUboDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device, &directionalLightUboAllocInfo, directionalLightUboDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = directionalLightsUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(DirectionalLightsUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = directionalLightUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 3;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		std::vector<VkDescriptorSetLayout> pointLightUboLayouts(MAX_FRAMES_IN_FLIGHT, mainRenderScenePipeline.descriptors[4].setLayout);
		VkDescriptorSetAllocateInfo pointLightUboAllocInfo{};
		pointLightUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		pointLightUboAllocInfo.descriptorPool = descriptorPool;
		pointLightUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		pointLightUboAllocInfo.pSetLayouts = pointLightUboLayouts.data();
			
		pointLightUboDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device, &pointLightUboAllocInfo, pointLightUboDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = pointLightsUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(PointLightsUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = pointLightUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 4;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		std::vector<VkDescriptorSetLayout> spotLightUboLayouts(MAX_FRAMES_IN_FLIGHT, mainRenderScenePipeline.descriptors[5].setLayout);
		VkDescriptorSetAllocateInfo spotLightUboAllocInfo{};
		spotLightUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		spotLightUboAllocInfo.descriptorPool = descriptorPool;
		spotLightUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		spotLightUboAllocInfo.pSetLayouts = spotLightUboLayouts.data();
			
		spotLightUboDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device, &spotLightUboAllocInfo, spotLightUboDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = spotLightsUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(SpotLightsUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = spotLightUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 5;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
		
		std::vector<VkDescriptorSetLayout> diffuseSamplerUboLayouts(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size(),
																	mainRenderScenePipeline.descriptors[6].setLayout);
		VkDescriptorSetAllocateInfo diffuseSamplerUboAllocInfo{};
		diffuseSamplerUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		diffuseSamplerUboAllocInfo.descriptorPool = descriptorPool;
		diffuseSamplerUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size());
		diffuseSamplerUboAllocInfo.pSetLayouts = diffuseSamplerUboLayouts.data();
		
		diffuseSamplerDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size());
		if (vkAllocateDescriptorSets(device, &diffuseSamplerUboAllocInfo, diffuseSamplerDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
				
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size(); ++i) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			unsigned int textureIndex = i / 2;
			imageInfo.imageView = textureImageViews[textureIndex];
			imageInfo.sampler = textureSamplers[textureIndex];
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = diffuseSamplerDescriptorSets[i];
			descriptorWrites[0].dstBinding = 6;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		std::vector<VkDescriptorSetLayout> specularSamplerUboLayouts(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size(),
																	 mainRenderScenePipeline.descriptors[7].setLayout);
		VkDescriptorSetAllocateInfo specularSamplerUboAllocInfo{};
		specularSamplerUboAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		specularSamplerUboAllocInfo.descriptorPool = descriptorPool;
		specularSamplerUboAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size());
		specularSamplerUboAllocInfo.pSetLayouts = specularSamplerUboLayouts.data();
		
		specularSamplerDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size());
		if (vkAllocateDescriptorSets(device, &specularSamplerUboAllocInfo, specularSamplerDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
				
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size(); ++i) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			unsigned int textureIndex = i / 2;
			imageInfo.imageView = textureImageViews[textureIndex];
			imageInfo.sampler = textureSamplers[textureIndex];
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = specularSamplerDescriptorSets[i];
			descriptorWrites[0].dstBinding = 7;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		std::vector<VkDescriptorSetLayout> shadowMapSamplerLayouts(MAX_FRAMES_IN_FLIGHT, mainRenderScenePipeline.descriptors[8].setLayout);
		VkDescriptorSetAllocateInfo shadowMapSamplerAllocInfo{};
		shadowMapSamplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		shadowMapSamplerAllocInfo.descriptorPool = descriptorPool;
		shadowMapSamplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		shadowMapSamplerAllocInfo.pSetLayouts = shadowMapSamplerLayouts.data();
		
		directionalLightSamperDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device, &shadowMapSamplerAllocInfo, directionalLightSamperDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
				
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
//			unsigned int textureIndex = i / 2;
			imageInfo.imageView = directionalLightShadowMapDepthImageViews[0];
			imageInfo.sampler = directionalLightShadowMapTextureSamplers[0];
//			std::cout << "хуй: " << directionalLightShadowMapDepthImageViews[0] << std::endl;
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = directionalLightSamperDescriptorSets[i];
			descriptorWrites[0].dstBinding = 8;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void CVulkanRenderer::updateDirectionalLightShadowMapDescriptorSets() {
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * directionalLightShadowMapMatrixUboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = shadowMapDirectionalLightModelMatrixUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(DirectionalLightShadowMapMatrixUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = shadowMapDirectionalLightDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
	
    void CVulkanRenderer::updateDescriptorSets() {
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = modelMatrixUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(ModelMatrixUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = matrixUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * viewPositionUboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = viewPositionUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(ViewPositionUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = viewPositionUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 1;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * materialUboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = materialUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(MaterialUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = materialUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 2;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = directionalLightsUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(DirectionalLightsUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = directionalLightUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 3;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = pointLightsUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(PointLightsUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = pointLightUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 4;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = spotLightsUniformBuffers[i];
			modelMatrixBufferInfo.offset = 0;
			modelMatrixBufferInfo.range = sizeof(SpotLightsUBO);
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = spotLightUboDescriptorSets[i];
			descriptorWrites[0].dstBinding = 5;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size(); ++i) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			unsigned int textureIndex = i / 2;
			imageInfo.imageView = textureImageViews[textureIndex];
			imageInfo.sampler = textureSamplers[textureIndex];
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = diffuseSamplerDescriptorSets[i];
			descriptorWrites[0].dstBinding = 6;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size(); ++i) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			unsigned int textureIndex = i / 2;
			imageInfo.imageView = textureImageViews[textureIndex];
			imageInfo.sampler = textureSamplers[textureIndex];
			
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = specularSamplerDescriptorSets[i];
			descriptorWrites[0].dstBinding = 7;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
//			unsigned int textureIndex = i / 2;
			imageInfo.imageView = directionalLightShadowMapDepthImageViews[0];
			imageInfo.sampler = directionalLightShadowMapTextureSamplers[0];

			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = directionalLightSamperDescriptorSets[i];
			descriptorWrites[0].dstBinding = 8;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
	
	void CVulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    VkCommandBuffer CVulkanRenderer::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void CVulkanRenderer::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void CVulkanRenderer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    uint32_t CVulkanRenderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void CVulkanRenderer::createShadowMapCommandBuffers() {
        shadowMapCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = shadowMapCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) shadowMapCommandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, shadowMapCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
	
    void CVulkanRenderer::createCommandBuffers() {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT * initializeTextureData_.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void CVulkanRenderer::recordShadowMapCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

		VkClearValue shadowMapClearValues[1];
		shadowMapClearValues[0].depthStencil.depth = 1.0f;
		shadowMapClearValues[0].depthStencil.stencil = 0;

		VkRenderPassBeginInfo shadowMapRenderPassInfo{};
		shadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		shadowMapRenderPassInfo.pNext = NULL;
		shadowMapRenderPassInfo.renderPass = directionalLightShadowMapRenderPass;
		shadowMapRenderPassInfo.framebuffer = directionalLightShadowMapFrameBuffers[imageIndex];
		shadowMapRenderPassInfo.renderArea.offset.x = 0;
		shadowMapRenderPassInfo.renderArea.offset.y = 0;
		shadowMapRenderPassInfo.renderArea.extent.width = swapChainExtent.width;
		shadowMapRenderPassInfo.renderArea.extent.height = swapChainExtent.height;
		shadowMapRenderPassInfo.clearValueCount = 1;
		shadowMapRenderPassInfo.pClearValues = shadowMapClearValues;

		vkCmdBeginRenderPass(commandBuffer, &shadowMapRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport shadowMapViewPort;
		shadowMapViewPort.height = swapChainExtent.height;
		shadowMapViewPort.width = swapChainExtent.width;
		shadowMapViewPort.minDepth = 0.0f;
		shadowMapViewPort.maxDepth = 1.0f;
		shadowMapViewPort.x = 0;
		shadowMapViewPort.y = 0;
		vkCmdSetViewport(commandBuffer, 0, 1, &shadowMapViewPort);

		VkRect2D shadowMapScissor;
		shadowMapScissor.extent.width = swapChainExtent.width;
		shadowMapScissor.extent.height = swapChainExtent.height;
		shadowMapScissor.offset.x = 0;
		shadowMapScissor.offset.y = 0;
		vkCmdSetScissor(commandBuffer, 0, 1, &shadowMapScissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowMapPipeline);

		const VkDeviceSize shadowMapOffsets[1] = { 0 };
		namespace cm = GLVM::ecs::components;
		core::vector<Entity> shadowMapLinkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::material,
																						   cm::vertex>();

		for ( unsigned int i = 0; i < shadowMapLinkedEntities.GetSize(); ++i ) {
			unsigned int entity = shadowMapLinkedEntities[i];
		unsigned int vertexID = componentManager->GetComponent<ecs::components::vertex>(entity)->vkVertexId_;
		VkBuffer shadowMapVertexBuffers[] = {vertexBufferContainer[vertexID]};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, shadowMapVertexBuffers, shadowMapOffsets);
		}

		vkCmdEndRenderPass(commandBuffer);
		
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
	
    void CVulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

		VkClearValue shadowMapClearValues[1];
		shadowMapClearValues[0].depthStencil.depth = 1.0f;
		shadowMapClearValues[0].depthStencil.stencil = 0;

		VkRenderPassBeginInfo shadowMapRenderPassInfo{};
		shadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		shadowMapRenderPassInfo.pNext = NULL;
		shadowMapRenderPassInfo.renderPass = directionalLightShadowMapRenderPass;
		shadowMapRenderPassInfo.framebuffer = directionalLightShadowMapFrameBuffers[currentFrame];
		shadowMapRenderPassInfo.renderArea.offset.x = 0;
		shadowMapRenderPassInfo.renderArea.offset.y = 0;
		shadowMapRenderPassInfo.renderArea.extent.width = swapChainExtent.width;
		shadowMapRenderPassInfo.renderArea.extent.height = swapChainExtent.height;
		shadowMapRenderPassInfo.clearValueCount = 1;
		shadowMapRenderPassInfo.pClearValues = shadowMapClearValues;

		vkCmdBeginRenderPass(commandBuffer, &shadowMapRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport shadowMapViewPort;
		shadowMapViewPort.height = swapChainExtent.height;
		shadowMapViewPort.width = swapChainExtent.width;
		shadowMapViewPort.minDepth = 0.0f;
		shadowMapViewPort.maxDepth = 1.0f;
		shadowMapViewPort.x = 0;
		shadowMapViewPort.y = 0;
		vkCmdSetViewport(commandBuffer, 0, 1, &shadowMapViewPort);

		VkRect2D shadowMapScissor;
		shadowMapScissor.extent.width = swapChainExtent.width;
		shadowMapScissor.extent.height = swapChainExtent.height;
		shadowMapScissor.offset.x = 0;
		shadowMapScissor.offset.y = 0;
		vkCmdSetScissor(commandBuffer, 0, 1, &shadowMapScissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipeline);

//		const VkDeviceSize shadowMapOffsets[1] = { 0 };
		namespace cm = GLVM::ecs::components;
		core::vector<Entity> directionalLightEntities      = componentManager->collectLinkedEntities<cm::transform,
																									 cm::directionalLight,
																									 cm::vertex>();

		namespace cm = GLVM::ecs::components;

		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::material,
																						   cm::vertex>();
		
		core::vector<Entity> viewPositionLinkedEntities = componentManager->collectLinkedEntities<cm::beholder>();
		cm::transform* playerTransformComponent = componentManager->GetComponent<cm::transform>(viewPositionLinkedEntities[0]);

		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			//              unsigned int uiEntity = (*entitiesContainerTexture)[j];
			unsigned int uiEntity = linkedEntities[i];
//			unsigned int uiDirectionalLightEntity = directionalLightEntities[0];
			unsigned int uiVertexId = componentManager->GetComponent<ecs::components::vertex>(uiEntity)->vkVertexId_;
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(uiEntity);
			// cm::directionalLight* directionalLightComponent = componentManager->GetComponent<cm::directionalLight>(uiDirectionalLightEntity);
			// unsigned int diffuseTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->diffuseTextureID_;
			// unsigned int specularTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->specularTextureID_;
			// cm::material* materialComponent = componentManager->GetComponent<cm::material>(uiEntity);

			// VkBufferMemoryBarrier bufferMemoryBarrier{};
			// bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			// bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			// bufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			// bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			// bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			// bufferMemoryBarrier.buffer = modelMatrixUniformBuffers[MAX_FRAMES_IN_FLIGHT * i + currentFrame];
			// bufferMemoryBarrier.offset = 0;
			// bufferMemoryBarrier.size   = VK_WHOLE_SIZE;

			// vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			// 					 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
			/// TODO: Second line work with no MAX_FRAMES_IN_FLIGHT define. Its litle bit wierd. Need to figure out why so.
			unsigned int uboIndex = MAX_FRAMES_IN_FLIGHT * i + currentFrame;
			// updateShadowMapMatrixUniformBuffer(currentFrame, transformComponent, directionalLightComponent);
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 0, 1, &shadowMapDirectionalLightDescriptorSets[currentFrame], 0, nullptr);
			updateMatrixUniformBuffer(uboIndex, transformComponent);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 0, 1, &matrixUboDescriptorSets[uboIndex], 0, nullptr);			
			// updateViewPositionUniformBuffer(currentFrame, playerTransformComponent);
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 1, 1, &viewPositionUboDescriptorSets[currentFrame], 0, nullptr);
			// updateMaterialUniformBuffer(uboIndex, materialComponent);
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 2, 1, &materialUboDescriptorSets[uboIndex], 0, nullptr);
			// updateDirectionalLightUniformBuffer(currentFrame);
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 3, 1, &directionalLightUboDescriptorSets[currentFrame], 0, nullptr);
			// updatePointLightUniformBuffer(currentFrame);
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 4, 1, &pointLightUboDescriptorSets[currentFrame], 0, nullptr);
			// updateSpotLightUniformBuffer(currentFrame);
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 5, 1, &spotLightUboDescriptorSets[currentFrame], 0, nullptr);
			
			// unsigned int textureID = componentManager->GetComponent<ecs::components::texture>(uiEntity)->id;
			// std::cout << "texture: " << textureID << std::endl;                
			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT16);

			unsigned int indicesContainerSize = aVertices_[uiVertexId].size();
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 6, 1, &diffuseSamplerDescriptorSets[MAX_FRAMES_IN_FLIGHT * diffuseTextureIndex + currentFrame], 0, nullptr);
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 7, 1, &specularSamplerDescriptorSets[MAX_FRAMES_IN_FLIGHT * specularTextureIndex + currentFrame], 0, nullptr);
			// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, directionalLightPipeline.pipelineLayout, 8, 1, &directionalLightSamperDescriptorSets[currentFrame], 0, nullptr);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}
		
		// for ( unsigned int i = 0; i < directionalLightEntities.GetSize(); ++i ) {
		// 	unsigned int entity = directionalLightEntities[i];
		// 	unsigned int vertexID = componentManager->GetComponent<ecs::components::vertex>(entity)->vkVertexId_;
		// 	VkBuffer shadowMapVertexBuffers[] = {vertexBufferContainer[vertexID]};
		// 	vkCmdBindVertexBuffers(commandBuffer, 0, 1, shadowMapVertexBuffers, shadowMapOffsets);

		// 	vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[vertexID], 0, VK_INDEX_TYPE_UINT16);

		// 	cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(entity);
		// 	cm::directionalLight* directionalLightComponent = componentManager->GetComponent<cm::directionalLight>(entity);

		// 	unsigned int indicesContainerSize = aVertices_[vertexID].size();
		// 	updateShadowMapMatrixUniformBuffer(currentFrame, transformComponent, directionalLightComponent);
		// 	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowMapPipelineLayout, 0, 1, &shadowMapMatrixUboDescriptorSets[currentFrame], 0, nullptr);
		// 	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		// }

		vkCmdEndRenderPass(commandBuffer);
		
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        /* MAX_FRAMES_IN_FLIGHT doubles texture pool and then j + currentFrame * texturePool
         * choose specific texture. */

		// core::vector<unsigned int>* entitiesContainerTexture =
		// 	componentManager->GetEntityContainer<ecs::components::texture>();
//		unsigned int textureContainerSize = entitiesContainerTexture->GetSize();

//		unsigned int transformContainerSize = pEntity_Container_refTransform->GetSize();

// 		for ( unsigned int i = 0; i < texture_load_data_.size(); ++i ) {
// 			for (unsigned int j = 0; j < texture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
// //				unsigned int uiEntity = (*pEntity_Container_refTransform)[i];
// //				ecs::components::transform* transformComponent = pComponent_Manager->GetComponent<ecs::components::transform>(uiEntity);


//             }
// 		}
		


		
		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			//              unsigned int uiEntity = (*entitiesContainerTexture)[j];
			unsigned int uiEntity = linkedEntities[i];
			unsigned int uiVertexId = componentManager->GetComponent<ecs::components::vertex>(uiEntity)->vkVertexId_;
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(uiEntity);
			unsigned int diffuseTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->diffuseTextureID_;
			unsigned int specularTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->specularTextureID_;
			cm::material* materialComponent = componentManager->GetComponent<cm::material>(uiEntity);

			// VkBufferMemoryBarrier bufferMemoryBarrier{};
			// bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			// bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			// bufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			// bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			// bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			// bufferMemoryBarrier.buffer = modelMatrixUniformBuffers[MAX_FRAMES_IN_FLIGHT * i + currentFrame];
			// bufferMemoryBarrier.offset = 0;
			// bufferMemoryBarrier.size   = VK_WHOLE_SIZE;

			// vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			// 					 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
			/// TODO: Second line work with no MAX_FRAMES_IN_FLIGHT define. Its litle bit wierd. Need to figure out why so.
			unsigned int uboIndex = MAX_FRAMES_IN_FLIGHT * i + currentFrame;
			updateMatrixUniformBuffer(uboIndex, transformComponent);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 0, 1, &matrixUboDescriptorSets[uboIndex], 0, nullptr);
			updateViewPositionUniformBuffer(currentFrame, playerTransformComponent);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 1, 1, &viewPositionUboDescriptorSets[currentFrame], 0, nullptr);
			updateMaterialUniformBuffer(uboIndex, materialComponent);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 2, 1, &materialUboDescriptorSets[uboIndex], 0, nullptr);
			updateDirectionalLightUniformBuffer(currentFrame);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 3, 1, &directionalLightUboDescriptorSets[currentFrame], 0, nullptr);
			updatePointLightUniformBuffer(currentFrame);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 4, 1, &pointLightUboDescriptorSets[currentFrame], 0, nullptr);
			updateSpotLightUniformBuffer(currentFrame);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 5, 1, &spotLightUboDescriptorSets[currentFrame], 0, nullptr);

			
			// unsigned int textureID = componentManager->GetComponent<ecs::components::texture>(uiEntity)->id;
			// std::cout << "texture: " << textureID << std::endl;                
			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT16);

			unsigned int indicesContainerSize = aVertices_[uiVertexId].size();
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 6, 1, &diffuseSamplerDescriptorSets[MAX_FRAMES_IN_FLIGHT * diffuseTextureIndex + currentFrame], 0, nullptr);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 7, 1, &specularSamplerDescriptorSets[MAX_FRAMES_IN_FLIGHT * specularTextureIndex + currentFrame], 0, nullptr);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mainRenderScenePipeline.pipelineLayout, 8, 1, &directionalLightSamperDescriptorSets[currentFrame], 0, nullptr);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}

        // vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineHUD);

        // VkViewport viewportHUD{};
        // viewportHUD.x = 0.0f;
        // viewportHUD.y = 0.0f;
        // viewportHUD.width = (float) swapChainExtent.width;
        // viewportHUD.height = (float) swapChainExtent.height;
        // viewportHUD.minDepth = 0.0f;
        // viewportHUD.maxDepth = 1.0f;
        // vkCmdSetViewport(commandBuffer, 0, 1, &viewportHUD);

        // VkRect2D scissorHUD{};
        // scissorHUD.offset = {0, 0};
        // scissorHUD.extent = swapChainExtent;
        // vkCmdSetScissor(commandBuffer, 0, 1, &scissorHUD);

        // VkBuffer vertexBuffersHUD[] = {hudVertexBuffer};
        // VkDeviceSize offsetsHUD[] = {0};
        // vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffersHUD, offsetsHUD);

        // vkCmdBindIndexBuffer(commandBuffer, hudIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

        // unsigned int hudBaseCounterValue = texture_load_data_.size() * MAX_FRAMES_IN_FLIGHT * texturePool_;
        // for (unsigned int n = hudBaseCounterValue; n < hudBaseCounterValue + hudTexture_load_data_.size() * MAX_FRAMES_IN_FLIGHT; n = n + 2) {
        //     vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayoutHUD, 0, 1, &descriptorSets[n + currentFrame], 0, nullptr);
        //     vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(hudIndices.size()), 1, 0, 0, 0);
        // }
        
        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CVulkanRenderer::createShadowMapSyncObjects() {
        shadowMapImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        shadowMapRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        shadowMapInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &shadowMapImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &shadowMapRenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &shadowMapInFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }
	
    void CVulkanRenderer::createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void CVulkanRenderer::updateShadowMapMatrixUniformBuffer([[maybe_unused]] uint32_t currentImage, ecs::components::transform* _transformComponent, ecs::components::directionalLight* directionalLightComponent) {
		DirectionalLightShadowMapMatrixUBO modelMatrixUBO{};

		float nearPlaneFlatShadowMap = 1.0f;
		float farPlaneFlatShadowMap = 25.0f;
		mat4 directionalProjectionMatrixLight = ortho(-10.0f, 10.0f, -10.0f, 10.0f,
													  nearPlaneFlatShadowMap, farPlaneFlatShadowMap);
		
		vec3 positionVectorLight  = directionalLightComponent->position;
		vec3 directionVectorLight = directionalLightComponent->direction;
		mat4 viewMatrixLight = LookAtMain(positionVectorLight,
										  directionVectorLight,
										  { 0.0f, 1.0f, 0.0f });
		
        modelMatrixUBO.model[0][0] = _transformComponent->fScale;
        modelMatrixUBO.model[1][1] = _transformComponent->fScale;
        modelMatrixUBO.model[2][2] = _transformComponent->fScale;
        modelMatrixUBO.model[3][3] = 1.0;
        modelMatrixUBO.model[0][3] = _transformComponent->tPosition[0];
        modelMatrixUBO.model[1][3] = _transformComponent->tPosition[1];
        modelMatrixUBO.model[2][3] = _transformComponent->tPosition[2];
        modelMatrixUBO.model.SelfTensorTranspose();

		directionalProjectionMatrixLight[1][1] *= -1;
		modelMatrixUBO.lightSpaceMatrix = viewMatrixLight * directionalProjectionMatrixLight;

        void* modelMatrixData;
        vkMapMemory(device, shadowMapDirectionalLightModelMatrixUniformBuffersMemory[currentImage], 0,
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, shadowMapDirectionalLightModelMatrixUniformBuffersMemory[currentImage]);
    }
	
    void CVulkanRenderer::updateMatrixUniformBuffer(uint32_t currentImage, ecs::components::transform* _transformComponent) {
        ModelMatrixUBO modelMatrixUBO{};
		// ViewPositionUBO viewPositionUBO{};
		// MaterialUBO materialUBO{};
		// DirectionalLightsUBO directionalLightsUBO{};
		// PointLightsUBO pointLightsUBO{};
		// SpotLightsUBO spotLightsUBO{};

        modelMatrixUBO.model[0][0] = _transformComponent->fScale;
        modelMatrixUBO.model[1][1] = _transformComponent->fScale;
        modelMatrixUBO.model[2][2] = _transformComponent->fScale;
        modelMatrixUBO.model[3][3] = 1.0;
        modelMatrixUBO.model[0][3] = _transformComponent->tPosition[0];
        modelMatrixUBO.model[1][3] = _transformComponent->tPosition[1];
        modelMatrixUBO.model[2][3] = _transformComponent->tPosition[2];
        modelMatrixUBO.model.SelfTensorTranspose();

        modelMatrixUBO.view = viewMatrix;
        modelMatrixUBO.proj = projectionMatrix;

        // if (_transformComponent.hud) {
        //     ubo.view.SelfIdentity();
        //     ubo.proj.SelfIdentity();
        // }

        // if (currentImage >= texture_load_data_.size() * MAX_FRAMES_IN_FLIGHT) {
        //     modelMatrixUBO.view.SelfIdentity();
        //     modelMatrixUBO.proj.SelfIdentity();
        // }
        
        modelMatrixUBO.proj[1][1] *= -1;

        // UniformBufferObjectLight uboLight{};
		// uboLight.valueLight = 30;

		// UniformBufferObject2 ubo2{};
		// ubo2.value = 0.5f;
		
        void* modelMatrixData;
        vkMapMemory(device, modelMatrixUniformBuffersMemory[currentImage], 0,
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, modelMatrixUniformBuffersMemory[currentImage]);

        // void* viewPositionData;
        // vkMapMemory(device, viewPositionUniformBuffersMemory[currentImage], 0,
		// 			sizeof(viewPositionUBO), 0, &viewPositionData);
        // memcpy(viewPositionData, &viewPositionUBO, sizeof(viewPositionUBO));
        // vkUnmapMemory(device, viewPositionUniformBuffersMemory[currentImage]);
		
		// void* materialData;
        // vkMapMemory(device, materialUniformBuffersMemory[currentImage], 0,
		// 			sizeof(materialUBO), 0, &materialData);
        // memcpy(materialData, &materialUBO, sizeof(materialUBO));
        // vkUnmapMemory(device, materialUniformBuffersMemory[currentImage]);

		// void* directionalLightsData;
        // vkMapMemory(device, directionalLightsUniformBuffersMemory[currentImage], 0,
		// 			sizeof(directionalLightsUBO), 0, &directionalLightsData);
        // memcpy(directionalLightsData, &directionalLightsUBO, sizeof(directionalLightsUBO));
        // vkUnmapMemory(device, directionalLightsUniformBuffersMemory[currentImage]);

		// void* pointLightsData;
        // vkMapMemory(device, pointLightsUniformBuffersMemory[currentImage], 0,
		// 			sizeof(pointLightsUBO), 0, &pointLightsData);
        // memcpy(pointLightsData, &pointLightsUBO, sizeof(pointLightsUBO));
        // vkUnmapMemory(device, pointLightsUniformBuffersMemory[currentImage]);

		// void* spotLightsData;
        // vkMapMemory(device, spotLightsUniformBuffersMemory[currentImage], 0,
		// 			sizeof(spotLightsUBO), 0, &spotLightsData);
        // memcpy(spotLightsData, &spotLightsUBO, sizeof(spotLightsUBO));
        // vkUnmapMemory(device, spotLightsUniformBuffersMemory[currentImage]);
    }

	void CVulkanRenderer::updateViewPositionUniformBuffer(uint32_t currentImage, ecs::components::transform* transformComponent) {
		ViewPositionUBO viewPositionUBO{};

		viewPositionUBO.viewPosition = transformComponent->tPosition;
		
        void* data;
        vkMapMemory(device, viewPositionUniformBuffersMemory[currentImage], 0,
					sizeof(viewPositionUBO), 0, &data);
        memcpy(data, &viewPositionUBO, sizeof(viewPositionUBO));
        vkUnmapMemory(device, viewPositionUniformBuffersMemory[currentImage]);
	}

	void CVulkanRenderer::updateMaterialUniformBuffer(uint32_t currentImage, ecs::components::material* materialComponent) {
		MaterialUBO materialUBO{};

		materialUBO.ambient = materialComponent->ambient;
		materialUBO.shininess = materialComponent->shininess;
//		std::cout << materialUBO.shininess << std::endl;
        void* data;
        vkMapMemory(device, materialUniformBuffersMemory[currentImage], 0,
					sizeof(MaterialUBO), 0, &data);
        memcpy(data, &materialUBO, sizeof(MaterialUBO));
        vkUnmapMemory(device, materialUniformBuffersMemory[currentImage]);
	}

	void CVulkanRenderer::updateDirectionalLightUniformBuffer(uint32_t currentImage) {
		DirectionalLightsUBO directionalLightUbo{};
		DirectionalLight directionalLight{};

		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::directionalLight,
																						   cm::vertex>();

		assert(directionalLightUboDescriptorsNumber < 4 && "Directional lights number greater then 4");

		for ( unsigned int i = 0; i < directionalLightUboDescriptorsNumber; ++i ) {
			cm::directionalLight* directionalLightComponent = componentManager->GetComponent<cm::directionalLight>(linkedEntities[i]);
			
			directionalLight.position  = directionalLightComponent->position;
			directionalLight.direction = directionalLightComponent->direction;
			directionalLight.ambient   = directionalLightComponent->ambient;
			directionalLight.diffuse   = directionalLightComponent->diffuse;
			directionalLight.specular  = directionalLightComponent->specular;

			directionalLightUbo.directionalLights[i] = directionalLight;			
		}

		directionalLightUbo.directionalLightsArraySize = directionalLightUboDescriptorsNumber;

        void* data;
        vkMapMemory(device, directionalLightsUniformBuffersMemory[currentImage], 0,
					sizeof(DirectionalLightsUBO), 0, &data);
        memcpy(data, &directionalLightUbo, sizeof(DirectionalLightsUBO));
        vkUnmapMemory(device, directionalLightsUniformBuffersMemory[currentImage]);
	}

	void CVulkanRenderer::updatePointLightUniformBuffer(uint32_t currentImage) {
		PointLightsUBO pointLightUboArray{};
		PointLight pointLightUBO{};

		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::pointLight,
																						   cm::vertex>();

		assert(pointLightUboDescriptorsNumber < 32 && "Point lights number greater then 32");
		for ( unsigned int i = 0; i < pointLightUboDescriptorsNumber; ++i ) {
			cm::pointLight* pointLightComponent = componentManager->GetComponent<cm::pointLight>(linkedEntities[i]);
			
			pointLightUBO.position  = pointLightComponent->position;
			pointLightUBO.ambient   = pointLightComponent->ambient;
			pointLightUBO.diffuse   = pointLightComponent->diffuse;
			pointLightUBO.specular  = pointLightComponent->specular;
			pointLightUBO.constant  = pointLightComponent->constant;
			pointLightUBO.linear    = pointLightComponent->linear;
			pointLightUBO.quadratic = pointLightComponent->quadratic; 

			pointLightUboArray.pointLights[i] = pointLightUBO;
		}

		pointLightUboArray.pointLightsArraySize = pointLightUboDescriptorsNumber;

        void* data;
        vkMapMemory(device, pointLightsUniformBuffersMemory[currentImage], 0,
					sizeof(pointLightUBO) * 32 + sizeof(pointLightUboDescriptorsNumber), 0, &data);
        memcpy(data, &pointLightUboArray, sizeof(pointLightUBO) * 32 + sizeof(pointLightUboDescriptorsNumber));
        vkUnmapMemory(device, pointLightsUniformBuffersMemory[currentImage]);
	}

	void CVulkanRenderer::updateSpotLightUniformBuffer(uint32_t currentImage) {
		SpotLightsUBO spotLightUBO{};
		SpotLight spotLight{};

		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::spotLight,
																						   cm::vertex>();

		assert(spotLightUboDescriptorsNumber < 8 && "Spot light number greater then 8");
		for ( unsigned int i = 0; i < spotLightUboDescriptorsNumber; ++i ) {
			cm::spotLight* spotLightComponent = componentManager->GetComponent<cm::spotLight>(linkedEntities[i]);
			
			spotLight.position    = spotLightComponent->position;
			spotLight.direction   = spotLightComponent->direction;
			spotLight.cutOff      = std::cos(Radians(spotLightComponent->cutOff));
			spotLight.outerCutOff = std::cos(Radians(spotLightComponent->outerCutOff));
			spotLight.ambient     = spotLightComponent->ambient;
			spotLight.diffuse     = spotLightComponent->diffuse;
			spotLight.specular    = spotLightComponent->specular;
			spotLight.constant    = spotLightComponent->constant;
			spotLight.linear      = spotLightComponent->linear;
			spotLight.quadratic   = spotLightComponent->quadratic; 

			spotLightUBO.spotLights[i] = spotLight;
		}

		spotLightUBO.spotLightArraySize = spotLightUboDescriptorsNumber;

        void* data;
        vkMapMemory(device, spotLightsUniformBuffersMemory[currentImage], 0,
					sizeof(spotLight) * 8 + sizeof(spotLightUboDescriptorsNumber), 0, &data);
        memcpy(data, &spotLightUBO, sizeof(spotLight) * 8 + sizeof(spotLightUboDescriptorsNumber));
        vkUnmapMemory(device, spotLightsUniformBuffersMemory[currentImage]);
	}
	
    void CVulkanRenderer::drawFrame() {
		namespace cm = GLVM::ecs::components;
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

//         ecs::ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
//         core::vector<cm::transform>* pEntity_Container_refTransform =
// 			pComponent_Manager->GetComponentContainer<cm::transform>();
// //		unsigned int transformContainerSize = pEntity_Container_refTransform->GetSize();

// 		for ( unsigned int i = 0; i < texture_load_data_.size(); ++i ) {
// 			for (unsigned int j = 0; j < texture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
// 				std::cout << "i: " << i << " j: " << j << std::endl;
// //				unsigned int uiEntity = (*pEntity_Container_refTransform)[i];
// //				ecs::components::transform* transformComponent = pComponent_Manager->GetComponent<ecs::components::transform>(uiEntity);
// 				ecs::components::transform transformComponent = (*pEntity_Container_refTransform)[texture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j]];
//                 updateUniformBuffer(MAX_FRAMES_IN_FLIGHT * i + currentFrame, transformComponent);
//             }
// 		}

        // Needed to skip images that intended to game objects that counts according to *texturePool_*.
        
        // unsigned int hudBaseCounterValue = texture_load_data_.size() * MAX_FRAMES_IN_FLIGHT * texturePool_;
        // unsigned int hudCounter = 0;
        // for (unsigned int n = hudBaseCounterValue; n < hudBaseCounterValue + hudTexture_load_data_.size(); n = n + 2) {

		// 	/// TODO: add for loop for inner entitiesownsthistypeoftexture.
        //     cm::transform transformComponent = (*pEntity_Container_refTransform)[hudTexture_load_data_[hudCounter].entitiesOwnsThisTypeOfTexture_[0]];
        //     updateUniformBuffer(n + currentFrame, transformComponent);
        //     ++hudCounter;
        // }
        vkResetFences(device, 1, &inFlightFences[currentFrame]);
        vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    VkShaderModule CVulkanRenderer::createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    VkSurfaceFormatKHR CVulkanRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR CVulkanRenderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D CVulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            // int width = 0, height = 0;
            // glfwGetFramebufferSize(window, &width, &height);

            // VkExtent2D actualExtent = {
            //     static_cast<uint32_t>(width),
            //     static_cast<uint32_t>(height)
            // };

			VkExtent2D actualExtent;
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    SwapChainSupportDetails CVulkanRenderer::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        
        return details;
    }

    bool CVulkanRenderer::isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    bool CVulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices CVulkanRenderer::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    std::vector<const char*> CVulkanRenderer::getRequiredExtensions() {
#ifdef VK_USE_PLATFORM_XLIB_KHR
        std::vector<const char*> pRequiredExtentions = {"VK_KHR_xlib_surface",
            "VK_EXT_acquire_xlib_display", "VK_KHR_display", "VK_KHR_surface",
            "VK_EXT_direct_mode_display"};
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
        std::vector<const char*> pRequiredExtentions = {"VK_KHR_win32_surface",
            "VK_KHR_surface"};
#endif

        if (enableValidationLayers) {
            pRequiredExtentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return pRequiredExtentions;
    }

    bool CVulkanRenderer::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    std::vector<char> CVulkanRenderer::readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL CVulkanRenderer::debugCallback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, [[maybe_unused]] void* pUserData) {
		// (void) messageSeverity;
		// (void) messageType;
		// (void) pUserData;
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

	void CVulkanRenderer::setDebugObjectNames() {
		VkDebugUtilsObjectNameInfoEXT mainPipelineObjectInfo{};
		mainPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		mainPipelineObjectInfo.pObjectName = "Main pipeline";
		mainPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		mainPipelineObjectInfo.objectHandle = (uint64_t)mainRenderScenePipeline.pipeline;
		SetDebugObjectName(device, &mainPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT mainPipelineLayoutObjectInfo{};
		mainPipelineLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		mainPipelineLayoutObjectInfo.pObjectName = "Main pipeline layout";
		mainPipelineLayoutObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
		mainPipelineLayoutObjectInfo.objectHandle = (uint64_t)mainRenderScenePipeline.pipelineLayout;
		SetDebugObjectName(device, &mainPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT directionalLightPipelineObjectInfo{};
		directionalLightPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		directionalLightPipelineObjectInfo.pObjectName = "Directional light pipeline";
		directionalLightPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		directionalLightPipelineObjectInfo.objectHandle = (uint64_t)directionalLightPipeline.pipeline;
		SetDebugObjectName(device, &directionalLightPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT directionalLightPipelineLayoutObjectInfo{};
		directionalLightPipelineLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		directionalLightPipelineLayoutObjectInfo.pObjectName = "Directional light pipeline layout";
		directionalLightPipelineLayoutObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
		directionalLightPipelineLayoutObjectInfo.objectHandle = (uint64_t)directionalLightPipeline.pipelineLayout;
		SetDebugObjectName(device, &directionalLightPipelineLayoutObjectInfo);

		for ( unsigned long i = 0; i < directionalLightPipeline.descriptors.GetSize(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetLayoutObjectInfo{};
			descriptorSetLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string layoutName = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_LAYOUT_RED, " Directional light shadow map descriptor set layout # ", i);
			const char* strLayoutName = layoutName.c_str();
			descriptorSetLayoutObjectInfo.pObjectName = strLayoutName;
			descriptorSetLayoutObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
			descriptorSetLayoutObjectInfo.objectHandle = (uint64_t)directionalLightPipeline.descriptors[i].setLayout;
			SetDebugObjectName(device, &descriptorSetLayoutObjectInfo);			
		}

		for ( unsigned long i = 0; i < mainRenderScenePipeline.descriptors.GetSize(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetLayoutObjectInfo{};
			descriptorSetLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string layoutName = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_LAYOUT_RED, " Main render descriptor set layout # ", i);
			const char* strLayoutName = layoutName.c_str();
			descriptorSetLayoutObjectInfo.pObjectName = strLayoutName;
			descriptorSetLayoutObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
			descriptorSetLayoutObjectInfo.objectHandle = (uint64_t)mainRenderScenePipeline.descriptors[i].setLayout;
			SetDebugObjectName(device, &descriptorSetLayoutObjectInfo);			
		}
		
		for ( unsigned long i = 0; i < shadowMapDirectionalLightDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Shadow map descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)shadowMapDirectionalLightDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

 		for ( unsigned long i = 0; i < matrixUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render model matrix descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)matrixUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < viewPositionUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render view position descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)viewPositionUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < materialUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render material descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)materialUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < directionalLightUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render directional light descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)directionalLightUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < pointLightUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render point light descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)pointLightUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < spotLightUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render spot light descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)spotLightUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < diffuseSamplerDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render diffuse sampler descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)diffuseSamplerDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

 		for ( unsigned long i = 0; i < specularSamplerDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render specular sampler descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)specularSamplerDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < shadowMapDirectionalLightDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render shadow map directional light descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)shadowMapDirectionalLightDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}
	}
}

// class RenderManager
// {
//     HelloTriangleApplication* vk_;
    
// public:
//     RenderManager(std::vector<Texture> _initializeTextureData) {
//         vk_ = new HelloTriangleApplication(_initializeTextureData);
//         vk_->setTextureData(_initializeTextureData);
//         vk_->run();
//     }

//     void SetTexture(std::vector<Texture> _initializeTextureData) {
//         delete vk_;
//         vk_ = nullptr;
//         vk_ = new HelloTriangleApplication(_initializeTextureData);
//         vk_->setTextureData(_initializeTextureData);
//         vk_->run();
//     }

//     void Draw() {
//         vk_->draw();
//     }
// };

// int main() {
//     // HelloTriangleApplication app;

//     // app.setTextureData(witch_dat_len, witch_dat);
//     // app.run();

//     std::vector<Texture> initializeTextureData;
//     Texture tex0{chelik_dat_len, chelik_dat};
//     Texture tex1{witch_dat_len, witch_dat};
//     initializeTextureData.push_back(tex0);
//     initializeTextureData.push_back(tex1);
    
//     RenderManager app(initializeTextureData);

// //    app.SetTexture(chelik_dat_len, chelik_dat);
    
//     try {
//         while(1) {
//             app.Draw();
//         }
//     } catch (const std::exception& e) {
//         std::cerr << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }

//     return EXIT_SUCCESS;
// }

