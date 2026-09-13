// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "ComponentManager.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "GraphicAPI/UniqueShaderModule.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ControllerComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/InventorySlotComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/InterfaceComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "EntityManager.hpp"
#include "GraphicAPI/RenderConfig.hpp"
#include "GraphicAPI/RenderData.hpp"
#include "PGA.hpp"
#include "ShaderStructs.hpp"
#include "Texture.hpp"
#include "ThreadPool.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include "VkStructs.hpp"
#include "WavefrontObjParser.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <string>
#include <thread>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>

namespace GLVM::core {
    CVulkanRenderer::CVulkanRenderer(RenderAssets& assets, const RenderFrame& frame)
        : assets_(assets), frame_(frame) {}

    CVulkanRenderer::~CVulkanRenderer() {
        cleanup();
    }

    void CVulkanRenderer::draw() {
		mainRenderDrawFrame();
    }

    void CVulkanRenderer::recreateSwapChain() {
        vkDeviceWaitIdle(device);

#ifdef VK_USE_PLATFORM_XCB_KHR
		Window->configureWindow();
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
		Window->configureWindow();
#endif

        // A minimized Win32 surface can report zero before WM_SIZE is dispatched.
        // Keep the current resources until the surface is drawable again.
        const auto support = querySwapChainSupport(physicalDevice);
        const auto extent = chooseSwapExtent(support.capabilities);
        if (!extent.width || !extent.height)
            return;

        cleanupSwapChain();

        for (unsigned int binding = 1; binding <= 3; ++binding) {
            const auto id = resources_.descriptorSetsConfig[MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[binding];
            const auto& description = resources_.descriptorBindingsConfig[id];
            for (unsigned int i = 0; i < description.shaderDescriptorsNumber; ++i)
                clearVK_Image(resources_.GPUDescriptors[description.globalDescriptorOffset + i].GPUImage);
        }

        for (auto semaphore : renderFinishedSemaphores)
            vkDestroySemaphore(device, semaphore, nullptr);
        renderFinishedSemaphores.clear();

        if (vkResetDescriptorPool(device, descriptorPool, 0) != VK_SUCCESS)
            throw std::runtime_error("Failed to reset descriptor pool during resize");

        createSwapChain(support);
        createImageViews();
        createDepthResources();
		createDirectionalLightShadowMapDepthResources();
		createSpotLightShadowMapDepthResources();
		createPointLightShadowMapDepthResources();
		createFramebuffers();
		createMainRenderDescriptorSets();
        renderFinishedSemaphores.resize(swapChainImages.size());
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for (auto& semaphore : renderFinishedSemaphores)
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
                throw std::runtime_error("Failed to recreate presentation semaphore");
        std::cout << "Swapchain resized: " << swapChainExtent.width << "x" << swapChainExtent.height << std::endl;
    }

    void CVulkanRenderer::run() {
        directionalLightNumber = frame_.directionalLights.GetSize();
        spotLightNumber = frame_.spotLights.GetSize();
        pointLightNumber = frame_.pointLights.GetSize();
		VkConfigInitializer(resources_);
		descriptorSetBuilder(resources_);
		pipelineBuilder(resources_);
		renderPassesBuilder(resources_);
		for( int i = 0; i < 20; ++i ) {
//			std::cout << "descriptor offset: " << resources_.descriptorBindingsConfig[i].globalDescriptorOffset << std::endl;
		}

		renderThreadPool = std::make_unique<ThreadPool>(3);
		startTime = std::chrono::steady_clock::now();

        initWindow();
        initVulkan();
    }

    void CVulkanRenderer::initWindow() {
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		Window = initializeWaylandWindow();

		createWaylandSurfaceInfo.display = Window->display;
		createWaylandSurfaceInfo.surface = Window->wl_surface;

		if ( createWaylandSurfaceInfo.display == NULL )
			std::cout << "DISPLAY NULL" << std::endl;
		else if ( createWaylandSurfaceInfo.surface == NULL )
			std::cout << "SURFACE NULL" << std::endl;

		createWaylandSurfaceInfo.sType   = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
		createWaylandSurfaceInfo.pNext   = nullptr;
		createWaylandSurfaceInfo.flags   = 0;
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
		Window = new GLVM::core::WindowXVulkan();
        createXlibSurfaceInfo.dpy = Window->GetDisplay();
        createXlibSurfaceInfo.window = Window->GetWindow();

        createXlibSurfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        createXlibSurfaceInfo.pNext = nullptr;
        createXlibSurfaceInfo.flags = 0;
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
		Window = new GLVM::core::WindowXCBVulkan();
		createXcbSurfaceInfo.window = Window->GetWindow();
		createXcbSurfaceInfo.connection = Window->GetConnection();

		createXcbSurfaceInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		createXcbSurfaceInfo.pNext = nullptr;
		createXcbSurfaceInfo.flags = 0;
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
		Window = new GLVM::core::WindowWinVulkan();
        createWin32SurfaceInfo.hwnd = Window->GetModernWindowHWND();
        createWin32SurfaceInfo.hinstance = GetModuleHandleW(nullptr);

        createWin32SurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createWin32SurfaceInfo.pNext = nullptr;
        createWin32SurfaceInfo.flags = 0;
#endif
    }

    void CVulkanRenderer::initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain(querySwapChainSupport(physicalDevice));
        createImageViews();
        createMainRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
		createCommandPool(mainRenderCommandPool);
		const uint32_t secondaryBuffersCommandPoolsNumber = 3;
		secondaryBuffersCommandPools.resize(secondaryBuffersCommandPoolsNumber);
		for( uint32_t i = 0; i < secondaryBuffersCommandPools.size(); ++i ) {
			createCommandPool(secondaryBuffersCommandPools[i]);
		}
        createDepthResources();
		createDirectionalLightShadowMapDepthResources();
		createSpotLightShadowMapDepthResources();
		createPointLightShadowMapDepthResources();
        createFramebuffers();
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
		initializeVertexBuffersWithWavefrontData();
		initializeVertexBuffersWithGLTFData();
		initializeVertexBuffersWithFontData();

        createMainRenderUniformBuffers();
        createMainRenderDescriptorPool();
        createMainRenderDescriptorSets();
        std::cout << "Swapchain: " << swapChainExtent.width << "x" << swapChainExtent.height << std::endl;
		vkDebugUtils::setDebugObjectNames( device, vertexBufferContainer, indexBufferContainer, resources_,
											assets_.fontIndicesContainer, fontVertexBufferContainer, fontIndexBufferContainer);
        // createCommandBuffers(mainRenderCommandPool, directionalLightCommandBuffers);
		// createCommandBuffers(mainRenderCommandPool, spotLightCommandBuffers);
		// createCommandBuffers(mainRenderCommandPool, pointLightCommandBuffers);
		const uint32_t mainRenderCommandBuffersNumber = 1;
		createCommandBuffers(mainRenderCommandPool, mainRenderCommandBuffers,
							 mainRenderCommandBuffersNumber, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		createCommandBuffers(secondaryBuffersCommandPools[0], directionalLightSecondaryCommandBuffers,
							 directionalLightNumber, VK_COMMAND_BUFFER_LEVEL_SECONDARY);
		createCommandBuffers(secondaryBuffersCommandPools[1], spotLightSecondaryCommandBuffers,
							 spotLightNumber, VK_COMMAND_BUFFER_LEVEL_SECONDARY);
		createCommandBuffers(secondaryBuffersCommandPools[2], pointLightSecondaryCommandBuffers,
							 pointLightNumber * 6 * 16, VK_COMMAND_BUFFER_LEVEL_SECONDARY);
		// createSyncObjects(directionalLightShadowMapImageAvailableSemaphores,
		// 				  directionalLightShadowMapRenderFinishedSemaphores,
		// 				  directionalLightShadowMapInFlightFences);
		// createSyncObjects(spotLightShadowMapImageAvailableSemaphores,
		// 				  spotLightShadowMapRenderFinishedSemaphores,
		// 				  spotLightShadowMapInFlightFences);
		// createSyncObjects(pointLightShadowMapImageAvailableSemaphores,
		// 				  pointLightShadowMapRenderFinishedSemaphores,
		// 				  pointLightShadowMapInFlightFences);
        createSyncObjects(imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences);
    }

    void CVulkanRenderer::cleanupSwapChain() {
		vkDeviceWaitIdle(device);
        vkDestroyImageView(device, mainDepthImageView, nullptr);
		vkDestroyImage(device, mainDepthPipelineImage, nullptr);
		vkFreeMemory(device, mainDepthPipelineImageMemory, nullptr);

        for (VkFramebuffer& framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        for (VkFramebuffer& framebuffer : directionalLightShadowMapFrameBuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

		for (VkFramebuffer& framebuffer : spotLightShadowMapFrameBuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

		for (std::vector<VkFramebuffer>& inner_vector : pointLightShadowMapFrameBuffers) {
			for (VkFramebuffer& framebuffer : inner_vector) {
				vkDestroyFramebuffer(device, framebuffer, nullptr);
			}
        }

        for (VkImageView& imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
        mainDepthImageView = VK_NULL_HANDLE;
        mainDepthPipelineImage = VK_NULL_HANDLE;
        mainDepthPipelineImageMemory = VK_NULL_HANDLE;
        swapChain = VK_NULL_HANDLE;
        swapChainFramebuffers.clear();
        directionalLightShadowMapFrameBuffers.clear();
        spotLightShadowMapFrameBuffers.clear();
        pointLightShadowMapFrameBuffers.clear();
        swapChainImageViews.clear();
    }

    void CVulkanRenderer::cleanup() {
        renderThreadPool.reset();
        if (device) cleanupSwapChain();

        if (device) {
        for (auto& buffer : resources_.buffers) {
            vkDestroyBuffer(device, buffer.buffer, nullptr);
            vkFreeMemory(device, buffer.deviceMemory, nullptr);
        }
        for (auto& image : resources_.images) clearVK_Image(&image);

		for( size_t i = 0; i < collisionsWireframesVKBuffers.GetSize(); ++i ) {
			vkDestroyBuffer(device, collisionsWireframesVKBuffers[i], nullptr);
			vkFreeMemory(device, collisionsWireframesVKDeviceMemory[i], nullptr);
			vkDestroyBuffer(device, collisionsWireframesIndicesVKBuffers[i], nullptr);
			vkFreeMemory(device, collisionsWireframesIndicesVKDeviceMemory[i], nullptr);
		}

		vkDestroyBuffer(device, hudUniformBuffer, nullptr);
		vkFreeMemory(device, hudUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, fontUniformBuffer, nullptr);
		vkFreeMemory(device, fontUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, hudScreenUniformBuffer, nullptr);
		vkFreeMemory(device, hudScreenUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, uiUniformBuffer, nullptr);
		vkFreeMemory(device, uiUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, uiIconsUniformBuffer, nullptr);
		vkFreeMemory(device, uiIconsUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, shadowMapDirectionalLightModelMatrixUniformBuffer, nullptr);
		vkFreeMemory(device, shadowMapDirectionalLightModelMatrixUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, shadowMapPointLightModelMatrixUniformBuffer, nullptr);
		vkFreeMemory(device, shadowMapPointLightModelMatrixUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, shadowMapSpotLightModelMatrixUniformBuffer, nullptr);
		vkFreeMemory(device, shadowMapSpotLightModelMatrixUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, virtualTexturesUniformBuffer, nullptr);
		vkFreeMemory(device, virtualTexturesUniformBufferMemory, nullptr);

		for ( size_t j = 0; j < vertexBufferContainer.size(); ++j ) {
			vkDestroyBuffer(device, vertexBufferContainer[j], nullptr);
			vkFreeMemory(device, vertexBufferMemoryContainer[j], nullptr);
		}
		for ( size_t j = 0; j < indexBufferContainer.size(); ++j ) {
			vkDestroyBuffer(device, indexBufferContainer[j], nullptr);
			vkFreeMemory(device, indexBufferMemoryContaner[j], nullptr);
		}

        for (auto buffer : fontVertexBufferContainer) vkDestroyBuffer(device, buffer, nullptr);
        for (auto memory : fontVertexBufferMemoryContainer) vkFreeMemory(device, memory, nullptr);
        for (auto buffer : fontIndexBufferContainer) vkDestroyBuffer(device, buffer, nullptr);
        for (auto memory : fontIndexBufferMemoryContaner) vkFreeMemory(device, memory, nullptr);
		vkDestroyBuffer(device, modelMatrixUniformBuffer, nullptr);
		vkFreeMemory(device, modelMatrixUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, lightDataUniformBuffer, nullptr);
		vkFreeMemory(device, lightDataUniformBuffersMemory, nullptr);

		vkDeviceWaitIdle(device);

		for( unsigned int i = 0; i < resources_.renderPasses.GetSize(); ++i ) {
			vkDestroyRenderPass( device, resources_.renderPasses[i], nullptr );
		}

		for ( unsigned int i = 0; i < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++i ) {
			vkDestroyDescriptorSetLayout(device, resources_.descriptorSetsConfig[i].setLayout, nullptr);
		}
		for ( unsigned int i = 0; i < SpecificPipeline::PIPELINES_NUMBER; ++i ) {
			vkDestroyPipeline(device, resources_.pipelineConfigs[i].pipeline, nullptr);
			vkDestroyPipelineLayout(device, resources_.pipelineConfigs[i].pipelineLayout, nullptr);
		}

		vkDestroySampler(device, textureSampler, nullptr);
        for(unsigned int i = 0; i < textureImages.size(); ++i)
        {
            vkDestroySampler(device, textureImages[i].sampler, nullptr);
			for ( unsigned int j = 0; j < textureImages[i].views.size(); ++j )
				vkDestroyImageView(device, textureImages[i].views[j], nullptr);

			vkDestroyImage(device, textureImages[i].image, nullptr);
            vkFreeMemory(device, textureImages[i].deviceMemory, nullptr);
        }


        for (auto semaphore : imageAvailableSemaphores) vkDestroySemaphore(device, semaphore, nullptr);
        for (auto semaphore : renderFinishedSemaphores) vkDestroySemaphore(device, semaphore, nullptr);
        for (auto fence : inFlightFences) vkDestroyFence(device, fence, nullptr);
        vkDestroyCommandPool(device, directionalLightCommandPool, nullptr);
		vkDestroyCommandPool(device, spotLightCommandPool, nullptr);
		vkDestroyCommandPool(device, pointLightCommandPool, nullptr);
		vkDestroyCommandPool(device, mainRenderCommandPool, nullptr);
		vkDestroyCommandPool(device, fontCommandPool, nullptr);
		vkDestroyCommandPool(device, hudCommandPool, nullptr);
		vkDestroyCommandPool(device, hudScreenCommandPool, nullptr);
		vkDestroyCommandPool(device, uiCommandPool, nullptr);
		vkDestroyCommandPool(device, uiIconsCommandPool, nullptr);
		vkDestroyCommandPool(device, virtualTexturesCommandPool, nullptr);
		for( uint32_t i = 0; i < secondaryBuffersCommandPools.size(); ++i ) {
			vkDestroyCommandPool(device, secondaryBuffersCommandPools[i], nullptr);
		}
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		vkDeviceWaitIdle(device);
        vkDestroyDevice(device, nullptr);
        }

        if (instance && debugMessenger) {
			vkDebugUtils::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        if (instance) {
            vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyInstance(instance, nullptr);
        }
        if (Window) Window->Close();
#ifdef VK_USE_PLATFORM_WIN32_KHR
        delete Window;
        Window = nullptr;
#endif
    }

}
