// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "ComponentManager.hpp"
#include "GraphicAPI/Vulkan.hpp"
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
#include "UnixApi/WindowWaylandVulkan.hpp"
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

namespace GLVM::core
{    
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        static auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void CreateBeginDebugUtilsLabelEXT([[maybe_unused]] VkInstance instance, [[maybe_unused]] VkCommandBuffer commandBuffer, [[maybe_unused]] const VkDebugUtilsLabelEXT* labelInfo) {
#ifndef NDEBUG
        static auto func = (PFN_vkCmdBeginDebugUtilsLabelEXT) vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
            func(commandBuffer, labelInfo);
#endif
    }

    void CreateEndDebugUtilsLabelEXT([[maybe_unused]] VkInstance instance, [[maybe_unused]] VkCommandBuffer commandBuffer) {
#ifndef NDEBUG
        static auto func = (PFN_vkCmdEndDebugUtilsLabelEXT) vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT");
            func(commandBuffer);
#endif
    }
	
	VkResult SetDebugObjectName(VkDevice device, const VkDebugUtilsObjectNameInfoEXT* objectNameInfo) {
		static auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
		if (func != nullptr) {
			return func(device, objectNameInfo);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        static auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
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
		// mutex0.lock();
		// mutex1.lock();
		// mutex2.lock();
		// std::thread directionalLightShadowMapThread(&CVulkanRenderer::directionalLightShadowMapDrawFrame, this);
		// std::thread spotLightShadowMapThread(&CVulkanRenderer::spotLightShadowMapDrawFrame, this);
		// std::thread pointLightShadowMapThread(&CVulkanRenderer::pointLightShadowMapDrawFrame, this);
		// std::thread mainRenderThread(&CVulkanRenderer::mainRenderDrawFrame, this);

		directionalLightShadowMapDrawFrame();
		spotLightShadowMapDrawFrame();
		pointLightShadowMapDrawFrame();
//		fontDrawFrame();
//		hudDrawFrame();
		mainRenderDrawFrame();
		
		// #ifdef VK_USE_PLATFORM_XCB_KHR
        // vkDeviceWaitIdle(device);
		// #endif
		// directionalLightShadowMapThread.join();
		// spotLightShadowMapThread.join();
		// pointLightShadowMapThread.join();
		// mainRenderThread.join();
    }

    void CVulkanRenderer::loadWavefrontObj() {
        for (unsigned int m = 0; m < pathsArray_.size(); ++m) {
            CWaveFrontObjParser parser;
            CWaveFrontObjParser* wavefrontObjParser = &parser;
            
            wavefrontObjParser->ReadFile(pathsArray_[m]);
            wavefrontObjParser->ParseFile();

            aIndices_.emplace_back();
            aVertices_.emplace_back();
			highest_gltf_Y.emplace_back();
			highest_gltf_Y[m] = -999.999f;

			frames.Push({});
			jointMatricesPerMesh.Push({});
			allMeshMaxAbsoluteValues.Push({});
            
            unsigned int vertexIndex  = 0;
            unsigned int textureIndex = 0;
			unsigned int normalIndex  = 0;
            unsigned int faceVerticesSize = wavefrontObjParser->getFaces().GetSize();

			meshAxisLimitingValues.highest_x = -MAXFLOAT;
			meshAxisLimitingValues.lowest_x  = MAXFLOAT;
			meshAxisLimitingValues.highest_y = -MAXFLOAT;
			meshAxisLimitingValues.lowest_y  = MAXFLOAT;
			meshAxisLimitingValues.highest_z = -MAXFLOAT;
			meshAxisLimitingValues.lowest_z  = MAXFLOAT;
			
            for (unsigned int i = 0; i < faceVerticesSize; ++i)
                for (int j = 0; j < 3; ++j) {
                    vertexIndex     = wavefrontObjParser->getFaces()[i][0][j] - 1;
					aIndices_[m].push_back(i * 3 + j);
                    SVertex vertex  = wavefrontObjParser->getCoordinateVertices()[vertexIndex];
                    textureIndex    = wavefrontObjParser->getFaces()[i][1][j] - 1;
                    SVertex texture = wavefrontObjParser->getTextureVertices()[textureIndex];
					normalIndex     = wavefrontObjParser->getFaces()[i][2][j] - 1;
					SVertex normal  = wavefrontObjParser->getNormals()[normalIndex];

					vec4 jointIndices;
					vec4 weights;

					if ( vertex[1] > highest_gltf_Y[m] )
						highest_gltf_Y[m] = vertex[1];

					if ( vertex[0] < meshAxisLimitingValues.lowest_x ) {
						meshAxisLimitingValues.lowest_x = vertex[0];
					} else if ( vertex[0] > meshAxisLimitingValues.highest_x ) {
						meshAxisLimitingValues.highest_x = vertex[0];
					}

					if ( vertex[1] < meshAxisLimitingValues.lowest_y ) {
						meshAxisLimitingValues.lowest_y = vertex[1];
					} else if ( vertex[1] > meshAxisLimitingValues.highest_y ) {
						meshAxisLimitingValues.highest_y = vertex[1];
					}

					if ( vertex[2] < meshAxisLimitingValues.lowest_z ) {
						meshAxisLimitingValues.lowest_z = vertex[2];
					} else if ( vertex[2] > meshAxisLimitingValues.highest_z ) {
						meshAxisLimitingValues.highest_z = vertex[2];
					}
					
					jointIndices[0] = -1;
					jointIndices[1] = -1;
					jointIndices[2] = -1;
					jointIndices[3] = -1;

					weights[0] = 1.0f;
					weights[1] = 1.0f;
					weights[2] = 1.0f;
					weights[2] = 1.0f;
					
                    aVertices_[m].Push({{vertex[0], vertex[1], vertex[2]},
										{normal[0], normal[1], normal[2]},
										{texture[0], texture[1]},
										{jointIndices[0], jointIndices[1], jointIndices[2]},
										{weights[0], weights[1], weights[2]}});
                }
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x = (meshAxisLimitingValues.highest_x - meshAxisLimitingValues.lowest_x) / 2.0f;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y = (meshAxisLimitingValues.highest_y - meshAxisLimitingValues.lowest_y) / 2.0f;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z = (meshAxisLimitingValues.highest_z - meshAxisLimitingValues.lowest_z) / 2.0f;

			// std::cout << "WAVEFRONT" << std::endl;
			// std::cout << "max width: " << meshAxisLimitingValues.highest_x << std::endl;
			// std::cout << "min width: " << meshAxisLimitingValues.lowest_x << std::endl;
			// std::cout << "max height: " << meshAxisLimitingValues.highest_y << std::endl;
			// std::cout << "min height: " << meshAxisLimitingValues.lowest_y << std::endl;
			// std::cout << "max deep: " << meshAxisLimitingValues.highest_z << std::endl;
			// std::cout << "min deep: " << meshAxisLimitingValues.lowest_z << std::endl;
			
			// std::cout << "half width: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x << std::endl;
			// std::cout << "half height: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y << std::endl;
			// std::cout << "half deep: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z << std::endl;
            vertexBufferContainer.emplace_back();
            vertexBufferMemoryContainer.emplace_back();
            createVertexBuffer(vertexBufferContainer[m], vertexBufferMemoryContainer[m], aVertices_[m]);

            indexBufferContainer.emplace_back();
            indexBufferMemoryContaner.emplace_back();
            createIndexBuffer(indexBufferContainer[m], indexBufferMemoryContaner[m], aIndices_[m]);
			++wavefrontObjCounter;
        }
    }

	void CVulkanRenderer::EnlargeFrameAccumulator(float value) {
		namespace cm = GLVM::ecs::components;
		
		ecs::ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::animation, cm::transform, cm::mesh>();
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
		for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
			Entity currentEntity                = linkedEntities[i];
			cm::transform* transformComponent   = componentManager->GetComponent<cm::transform>(currentEntity);
			unsigned int mesh_id                = componentManager->GetComponent<cm::mesh>(currentEntity)->handle.id;

			if ( jointMatricesPerMesh.GetSize() > 0 && jointMatricesPerMesh[mesh_id].GetSize() > 0 )
				transformComponent->frameAccumulator += value;
		}
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


		// if ( hud_screen_x > 1.0f )
		// 	hud_screen_x = 1.0f;
		// else if ( hud_screen_x < -1.0f )
		// 	hud_screen_x = -1.0f;
		
		// if ( hud_screen_y > 1.0f )
		// 	hud_screen_y = 1.0f;
		// else if ( hud_screen_y < -1.0f )
		// 	hud_screen_y = -1.0f;
		
        fYaw = g_eEvent.mousePointerPosition.offset_X;
        fPitch = g_eEvent.mousePointerPosition.offset_Y;
        fYaw *= kSensitivity;
        fPitch *= kSensitivity;

        g_eEvent.mousePointerPosition.pitch = fPitch;
        g_eEvent.mousePointerPosition.yaw = fYaw;

		current_X = (float)g_eEvent.mousePointerPosition.offset_X;
		current_Y = (float)g_eEvent.mousePointerPosition.offset_Y;
		float delta_x = 0.0f;
		float delta_y = 0.0f;
		#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		delta_x = current_X;
		delta_y = current_Y;
		#else
		delta_x = current_X - prev_X;
		delta_y = current_Y - prev_Y;
		delta_y *= -1.0f;
		#endif
		// delta_x *= kSensitivity;
		// delta_y *= kSensitivity;
		
		const vec3 rightVec = Cross(cameraComponent.forward, cameraComponent.up);
		const vec3 newUpVec = Cross(rightVec, cameraComponent.forward);
		/*
		 * 1. The mouse direction determines the "intended direction of rotation" for the object.
		 * 2. The camera is "looking forward."
		 * 3. To make the object "rotate as if the mouse is pushing it," you need to rotate it around an axis that is perpendicular to both the view direction and the mouse movement.
		 */
		const vec3 rotateAxis = Normalize(Cross(cameraComponent.forward, rightVec * delta_x + newUpVec * delta_y));

		if ( VecLength(rotateAxis) >= 0.001f ) {
			/// A vector in the screen's tangent plane: it indicates the direction in which the mouse moved, but expressed in world (or 3D) space.
			float rotationAngle = sqrt(delta_y * delta_y + delta_x * delta_x);
			constexpr float angleScale = 0.1f;                                                                                     
			rotationAngle = Radians(rotationAngle * angleScale);
			constexpr float quatAngleCorrection = 0.5f;                                                                                 /// Quaternions need devision by 2
			[[maybe_unused]] const float sinRotationAngle = sinf(rotationAngle * quatAngleCorrection);
			// const Quaternion rotationQuat = Quaternion(cosf(rotationAngle * quatAngleCorrection), sinRotationAngle * rotateAxis[0],
			// 									 sinRotationAngle * rotateAxis[1], sinRotationAngle * rotateAxis[2]);
			// // const Quaternion appliedRotationQuat = multiplyQuaternion(multiplyQuaternion(rotationQuat, Quaternion(0.0f, cameraComponent.forward[0],
			// // 																								cameraComponent.forward[1], cameraComponent.forward[2])),
			// // 													conjugate(rotationQuat));

			// const Quaternion appliedRotationQuat = (rotationQuat * Quaternion(0.0f, cameraComponent.forward[0], cameraComponent.forward[1],
			// 																  cameraComponent.forward[2])) * conjugate(rotationQuat);

			
			// forward[0] = appliedRotationQuat.x;
			// forward[1] = appliedRotationQuat.y;
			// forward[2] = appliedRotationQuat.z;
			pga::point appliedRotationPoint = exp(rotationAngle * quatAngleCorrection, pga::rline{ .rx = -rotateAxis.m_vector[0],
					.ry = -rotateAxis.m_vector[1], .rz = -rotateAxis.m_vector[2]}) >> pga::point{ .x = cameraComponent.forward[0],
					.y = cameraComponent.forward[1], .z = cameraComponent.forward[2], .w = 1.0f };
			forward[0] = appliedRotationPoint.x;
			forward[1] = appliedRotationPoint.y;
			forward[2] = appliedRotationPoint.z;
		}
		cameraComponent.forward = Normalize(forward);
		_Player.forward = cameraComponent.forward;
		mat4 view = LookAtMain(cameraComponent.Position + _Player.position, cameraComponent.Position + _Player.position + cameraComponent.forward, cameraComponent.up);
		for ( unsigned int i = 0; i < 4; ++i )
			for ( unsigned int j = 0; j < 4; ++j )
				 viewMatrix_[i][j] = view[i][j];
		
		if ( !isInventoryOpened )
			viewMatrix = viewMatrix_;

		prev_Y = (float)g_eEvent.mousePointerPosition.offset_Y;
		prev_X = (float)g_eEvent.mousePointerPosition.offset_X;
    }

    void CVulkanRenderer::SetProjectionMatrix()
	{
		mat4 tProjection_Matrix = Perspective(Radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
		projectionMatrix = tProjection_Matrix;
		projectionMatrix[1][1] *= 1.0f;
	}
	
    void CVulkanRenderer::createTextureImage() {
		uint32_t texWidth, texHeight;
		[[maybe_unused]] uint32_t texChannels;

        for(unsigned int i = 0; i < initializeTextureData_.size(); ++i)
        {
			VkDeviceSize imageSize{};
			unsigned char* pixels;
			[[maybe_unused]] const char* path_to_stb_image = nullptr;

			#ifndef STB_IMAGE_IMPLEMENTATION
            imageSize = initializeTextureData_[i].dat_length_;
            pixels = initializeTextureData_[i].u_iData_;
            texWidth = initializeTextureData_[i].iWidth_;
            texHeight = initializeTextureData_[i].iHeight_;
			#endif

			#ifdef STB_IMAGE_IMPLEMENTATION
			path_to_stb_image = initializeTextureData_[i].path_to_image;
			pixels = stbi_load(path_to_stb_image, reinterpret_cast<int*>(&texWidth), reinterpret_cast<int*>(&texHeight),
							   reinterpret_cast<int*>(&texChannels), STBI_rgb_alpha);
			imageSize = texWidth * texHeight * 4;
			#endif

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
			VK_Image textureImage = {
				.image = VkImage{},
				.deviceMemory = VkDeviceMemory{},
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.createFlags  = 0,
				.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				.usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.arrayLayers = 1,
				.width = texWidth,
				.height = texHeight
			};

            createImage(textureImage);

            transitionImageLayout(textureImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            copyBufferToImage(stagingBuffer, textureImage.image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
            transitionImageLayout(textureImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			textureImages.push_back(textureImage);
			
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }
    }

    void CVulkanRenderer::recreateSwapChain() {
        vkDeviceWaitIdle(device);

#ifdef VK_USE_PLATFORM_XCB_KHR
		Window->configureWindow();
#endif

// #ifdef VK_USE_PLATFORM_WAYLAND_KHR
// 		Window.Close();
// 		Window = GLVM::core::WindowWaylandVulkan();
// 		// if ( buffer == NULL )
// 		// 	std::cout << "BUFFER V GOVNE" << std::endl;
			
// 		createWaylandSurfaceInfo.display = display;
// 		createWaylandSurfaceInfo.surface = wl_surface;

// 		createWaylandSurfaceInfo.sType   = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
// 		createWaylandSurfaceInfo.pNext   = nullptr;
// 		createWaylandSurfaceInfo.flags   = 0;
// #endif
		
        cleanupSwapChain();
		
        createSwapChain();
        createImageViews();
        createDepthResources();
		createDirectionalLightShadowMapDepthResources();
		createSpotLightShadowMapDepthResources();
		createPointLightShadowMapDepthResources();
		createFramebuffers();
		// createTextureImage();
        // createTextureImageView();
        // createTextureSampler();

		updateDirectionalLightShadowMapDescriptorSets();
		updateSpotLightShadowMapDescriptorSets();
		updatePointLightShadowMapDescriptorSets();
//		updateDescriptorSets();
		updateHudDescriptorSets();
		updateFontRenderDescriptorSets();
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
		descriptorSetBuilder();
		pipelineBuilder();
		renderPassesBuilder();
		for( int i = 0; i < 20; ++i ) {
			std::cout << "descriptor offset: " << descriptorBindingsConfig[i].globalDescriptorOffset << std::endl;
		}
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager   = ecs::ComponentManager::GetInstance();
		core::vector<Entity> directionalLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																									  cm::directionalLight,
																									  cm::mesh>();

		directionalLightNumber = directionalLightLinkedEntities.GetSize();

		core::vector<Entity> spotLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							   cm::spotLight,
																							   cm::mesh>();

		spotLightNumber = spotLightLinkedEntities.GetSize();

		core::vector<Entity> pointLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																								cm::pointLight,
																								cm::mesh>();

		pointLightNumber = pointLightLinkedEntities.GetSize();

		core::vector<Entity> actorsLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							cm::material,
																							cm::mesh>();

		actorsNumber = actorsLinkedEntities.GetSize();
		
        initWindow();
        initVulkan();
    }
    
    void CVulkanRenderer::initWindow() {
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		Window = initializeWaylandWindow();
			
		createWaylandSurfaceInfo.display = Window->display;
		createWaylandSurfaceInfo.surface = Window->wl_surface;
		aspectRate = (float)Window->width / (float)Window->height;

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
        createWin32SurfaceInfo.hwnd = Window.GetModernWindowHWND();
        
        createWin32SurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createWin32SurfaceInfo.pNext = nullptr;
        createWin32SurfaceInfo.flags = 0;
#endif
    }

	void CVulkanRenderer::initializeGLTF() {
		core::vector<bool> animationFlags;
		for (unsigned int m = 0; m < pathsGLTF_.GetSize(); ++m) {
			Core::CJsonParser jsonParser;
			aVertexesTemp_.emplace_back();
			aIndices_.emplace_back();
			frames.Push({});
			jointMatricesPerMesh.Push({});
			animationFlags.Push({});
			highest_gltf_Y.emplace_back();
			uint32_t nextIndexGLTF = wavefrontObjCounter + m;
			jsonParser.LoadGLTF(pathsGLTF_[m], aVertexesTemp_[m], aIndices_[nextIndexGLTF], jointMatricesPerMesh[nextIndexGLTF], frames[nextIndexGLTF], animationFlags[m], highest_gltf_Y[nextIndexGLTF]);
		}

		for (unsigned int m = 0; m < pathsGLTF_.GetSize(); ++m) {
//            aIndices_.emplace_back();
//            aVertices_.emplace_back();
			aVertices_.emplace_back();
			allMeshMaxAbsoluteValues.Push({});

			meshAxisLimitingValues.highest_x = -MAXFLOAT;
			meshAxisLimitingValues.lowest_x  = MAXFLOAT;
			meshAxisLimitingValues.highest_y = -MAXFLOAT;
			meshAxisLimitingValues.lowest_y  = MAXFLOAT;
			meshAxisLimitingValues.highest_z = -MAXFLOAT;
			meshAxisLimitingValues.lowest_z  = MAXFLOAT;
			
			int stepOffset = 0;
			if ( animationFlags[m] )
				stepOffset = 8;
			else
				stepOffset = 16;
			for ( unsigned int n = 0; n < aVertexesTemp_[m].size(); n += stepOffset ) {
				SVertex vertex;
				vertex[0] = aVertexesTemp_[m][n];
			    vertex[1] = aVertexesTemp_[m][n + 1];
				vertex[2] = aVertexesTemp_[m][n + 2];
				SVertex normal;
				normal[0] = aVertexesTemp_[m][n + 3];
				normal[1] = aVertexesTemp_[m][n + 4];
				normal[2] = aVertexesTemp_[m][n + 5];
				SVertex texture;
				texture[0] = aVertexesTemp_[m][n + 6];
				texture[1] = aVertexesTemp_[m][n + 7];

				if ( vertex[0] < meshAxisLimitingValues.lowest_x ) {
					meshAxisLimitingValues.lowest_x = vertex[0];
				} else if ( vertex[0] > meshAxisLimitingValues.highest_x ) {
					meshAxisLimitingValues.highest_x = vertex[0];
				}

				if ( vertex[1] < meshAxisLimitingValues.lowest_y ) {
					meshAxisLimitingValues.lowest_y = vertex[1];
				} else if ( vertex[1] > meshAxisLimitingValues.highest_y ) {
					meshAxisLimitingValues.highest_y = vertex[1];
				}

				if ( vertex[2] < meshAxisLimitingValues.lowest_z ) {
					meshAxisLimitingValues.lowest_z = vertex[2];
				} else if ( vertex[2] > meshAxisLimitingValues.highest_z ) {
					meshAxisLimitingValues.highest_z = vertex[2];
				}
				
				vec4 joinIndices;
				vec4 weights;
				if ( animationFlags[m] ) {
					joinIndices[0] = -1;
					joinIndices[1] = -1;
					joinIndices[2] = -1;
					joinIndices[3] = -1;

					weights[0] = 1;
					weights[1] = 1;
					weights[2] = 1;
					weights[3] = 1;
					
				} else {
					joinIndices[0] = aVertexesTemp_[m][n + 8];
					joinIndices[1] = aVertexesTemp_[m][n + 9];
					joinIndices[2] = aVertexesTemp_[m][n + 10];
					joinIndices[3] = aVertexesTemp_[m][n + 11];

					weights[0] = aVertexesTemp_[m][n + 12];
					weights[1] = aVertexesTemp_[m][n + 13];
					weights[2] = aVertexesTemp_[m][n + 14];
					weights[3] = aVertexesTemp_[m][n + 15];
				}

				uint32_t nextIndexGLTF = wavefrontObjCounter + m;
				aVertices_[nextIndexGLTF].Push({{vertex[0], vertex[1], vertex[2]},
										 {normal[0], normal[1], normal[2]},
										 {texture[0], texture[1]},
										 {joinIndices[0], joinIndices[1], joinIndices[2], joinIndices[3]},
										 {weights[0], weights[1], weights[2], weights[3]}});
			}
			uint32_t nextIndexGLTF = wavefrontObjCounter + m;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x = (meshAxisLimitingValues.highest_x - meshAxisLimitingValues.lowest_x) / 2.0f;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y = (meshAxisLimitingValues.highest_y - meshAxisLimitingValues.lowest_y) / 2.0f;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z = (meshAxisLimitingValues.highest_z - meshAxisLimitingValues.lowest_z) / 2.0f;

			// std::cout << "GLTF" << std::endl;
			// std::cout << "max width: " << meshAxisLimitingValues.highest_x << std::endl;
			// std::cout << "min width: " << meshAxisLimitingValues.lowest_x << std::endl;
			// std::cout << "max height: " << meshAxisLimitingValues.highest_y << std::endl;
			// std::cout << "min height: " << meshAxisLimitingValues.lowest_y << std::endl;
			// std::cout << "max deep: " << meshAxisLimitingValues.highest_z << std::endl;
			// std::cout << "min deep: " << meshAxisLimitingValues.lowest_z << std::endl;
			
			// std::cout << "half width: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x << std::endl;
			// std::cout << "half height: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y << std::endl;
			// std::cout << "half deep: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z << std::endl;
			
            vertexBufferContainer.emplace_back();
            vertexBufferMemoryContainer.emplace_back();
            createVertexBuffer(vertexBufferContainer[nextIndexGLTF], vertexBufferMemoryContainer[nextIndexGLTF], aVertices_[nextIndexGLTF]);

            indexBufferContainer.emplace_back();
            indexBufferMemoryContaner.emplace_back();
            createIndexBuffer(indexBufferContainer[nextIndexGLTF], indexBufferMemoryContaner[nextIndexGLTF], aIndices_[nextIndexGLTF]);
			++gltfCounter;
		}
	}

	void CVulkanRenderer::initializeGameLevelVertices() {
		for ( unsigned int m = 0; m < levelGeneratedVertices.size(); ++m ) {
			aVertices_.push_back(levelGeneratedVertices[m]);
			aIndices_.push_back(levelGeneratedIndices[m]);
			jointMatricesPerMesh.Push({});
			frames.Push({});
			for( int i = 0; i < 30; ++i ) {
				frames[frames.GetSize() - 1].Push(0.0f);
			}
			int maximumJoints     = 6;
			core::vector<core::vector<mat4>> jointMatrices;
			for ( int i = 0; i < maximumJoints; ++i) {
				core::vector<mat4>  globalAllFrameNodeMatrix;
				int numberOfFrames = 30;
				for ( int j = 0; j < numberOfFrames; ++j ) {
					mat4 unitMatrix(1.0f);
					globalAllFrameNodeMatrix.Push(unitMatrix);
				}

				jointMatrices.Push(globalAllFrameNodeMatrix);
			}
			jointMatricesPerMesh[jointMatricesPerMesh.GetSize() - 1] = jointMatrices;
			
			uint32_t nextIndexGLTF = wavefrontObjCounter + gltfCounter + m;
		
			vertexBufferContainer.emplace_back();
			vertexBufferMemoryContainer.emplace_back();
			createVertexBuffer(vertexBufferContainer[nextIndexGLTF], vertexBufferMemoryContainer[nextIndexGLTF], aVertices_[nextIndexGLTF]);

			indexBufferContainer.emplace_back();
			indexBufferMemoryContaner.emplace_back();
			createIndexBuffer(indexBufferContainer[nextIndexGLTF], indexBufferMemoryContaner[nextIndexGLTF], aIndices_[nextIndexGLTF]);
		}
	}
	
	void CVulkanRenderer::initializeFontData() {
		constexpr float fontStep = 1.0 / 12;
		constexpr unsigned int glyph_row = 7;
		constexpr unsigned int glyph_column = 12;

		fontVertexBufferContainer.resize(128);
		fontVertexBufferMemoryContainer.resize(128);

		fontIndexBufferContainer.resize(128);
		fontIndexBufferMemoryContaner.resize(128);
		
		for ( unsigned int i = 0; i < glyph_row; ++i )
			for ( unsigned int j = 0; j < glyph_column; ++j ) {
				core::vector<Vertex> symbol_g_vertices;
					symbol_g_vertices.Push({{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {fontStep * j, fontStep * i + fontStep}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}});
					symbol_g_vertices.Push({{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {fontStep * j + fontStep, fontStep * i + fontStep}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}});
					symbol_g_vertices.Push({{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {fontStep * j, fontStep * i}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}});
					symbol_g_vertices.Push({{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {fontStep * j + fontStep, fontStep * i}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}});
				unsigned int currentBufferIndex = i * glyph_column + j;

				bool exitFlag = false;
				const unsigned int nextBufferIndex = static_cast<const unsigned int>(glyphs[currentBufferIndex]);
				for ( unsigned int n = 0; n < fontIndicesContainer.size(); ++n ) {                 ///< TODO: Fix gabage algo
					if ( nextBufferIndex == fontIndicesContainer[n] )
						exitFlag = true;
				}

				if ( exitFlag )
					continue;
				
				fontIndicesContainer.push_back(nextBufferIndex);
				createVertexBuffer(fontVertexBufferContainer[nextBufferIndex], fontVertexBufferMemoryContainer[nextBufferIndex], symbol_g_vertices);

				createIndexBuffer(fontIndexBufferContainer[nextBufferIndex], fontIndexBufferMemoryContaner[nextBufferIndex], symbol_g_indices);

			}
	}

	
    void CVulkanRenderer::initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createMainRenderPass();
		createFontRenderPass();
		createHudRenderPass();
		createHudScreenRenderPass();
		createRenderPass_UI();
		createRenderPassIcons_UI();
		createDirectionalLightShadowMapRenderPass();
		createSpotLightShadowMapRenderPass();
		createPointLightShadowMapRenderPass();
		createVirtualTextureRenderPass();
		for ( int descriptorSetCounter = 0; descriptorSetCounter < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++descriptorSetCounter ) {
			std::cout << "ds #: " << descriptorSetCounter << std::endl;
			createDescriptorSetLayout(descriptorSetsConfig[descriptorSetCounter]);
		}
		for ( int graphicsPipelineCounter = 0; graphicsPipelineCounter < SpecificPipeline::PIPELINES_NUMBER; ++graphicsPipelineCounter ) {
			std::cout << "next pipeline: " << std::endl;
			createGraphicsPipeline( pipelineConfigs[graphicsPipelineCounter], renderPasses[graphicsPipelineCounter], VK_POLYGON_MODE_FILL );
		}
		createCommandPool(mainRenderCommandPool);
        createDepthResources();
		createDirectionalLightShadowMapDepthResources();
		createSpotLightShadowMapDepthResources();
		createPointLightShadowMapDepthResources();
        createFramebuffers();
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
        loadWavefrontObj();
		initializeGLTF();
		initializeFontData();
		
        createMainRenderUniformBuffers();
        createMainRenderDescriptorPool();
		createDirectionalLightShadowMapDescriptorSets();
		createSpotLightShadowMapDescriptorSets();
		createPointLightShadowMapDescriptorSets();
		createHudDescriptorSets();
		createHudScreenDescriptorSets();
		createDescriptorSets_UI();
		createDescriptorSetsIcons_UI();
		createFontRenderDescriptorSets();
        createMainRenderDescriptorSets();
		setDebugObjectNames();
        createCommandBuffers(mainRenderCommandPool, directionalLightCommandBuffers);
		createCommandBuffers(mainRenderCommandPool, spotLightCommandBuffers);
		createCommandBuffers(mainRenderCommandPool, pointLightCommandBuffers);
		createCommandBuffers(mainRenderCommandPool, mainRenderCommandBuffers);
		createSyncObjects(fontImageAvailableSemaphores,
						  fontRenderFinishedSemaphores,
						  fontInFlightFences);
		createSyncObjects(hudImageAvailableSemaphores,
						  hudRenderFinishedSemaphores,
						  hudInFlightFences);
		createSyncObjects(directionalLightShadowMapImageAvailableSemaphores,
						  directionalLightShadowMapRenderFinishedSemaphores,
						  directionalLightShadowMapInFlightFences);
		createSyncObjects(spotLightShadowMapImageAvailableSemaphores,
						  spotLightShadowMapRenderFinishedSemaphores,
						  spotLightShadowMapInFlightFences);
		createSyncObjects(pointLightShadowMapImageAvailableSemaphores,
						  pointLightShadowMapRenderFinishedSemaphores,
						  pointLightShadowMapInFlightFences);
        createSyncObjects(imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences);
		createSyncObjects(virtualTexturesImageAvailableSemaphores, virtualTexturesRenderFinishedSemaphores, virtualTexturesInFlightFences);
    }

	void CVulkanRenderer::clearPipeline( core::vector<VK_Image>& textureImages ) {
		// for ( unsigned int n = 0; n < pipeline.descriptorSets.GetSize(); ++n ) {
		// 	DescriptorSet currentDescriptorSet = pipeline.descriptorSets[n];
		// 	for ( unsigned int m = 0; m < currentDescriptorSet.descriptorBindings.GetSize(); ++m ) {
//			std::cout << "size of main descriptors: " << mainRenderScenePipeline.descriptors.GetSize() << std::endl;
				for(unsigned int i = 0; i < textureImages.GetSize(); ++i)
					{
//					std::cout << "size of main descriptors: " << mainRenderScenePipeline.descriptors[m].uniformBuffers.size() << std::endl;
						vkDestroySampler(device, textureImages[i].sampler, nullptr);
						for ( unsigned int j = 0; j < textureImages[i].views.size(); ++j )
							vkDestroyImageView(device, textureImages[i].views[j], nullptr);

						textureImages[i].views.clear();
					
						vkDestroyImage(device, textureImages[i].image, nullptr);
						vkFreeMemory(device, textureImages[i].deviceMemory, nullptr);
						// for ( unsigned int j = 0; j < mainRenderScenePipeline.descriptors[m].uniformBuffersMemory.size(); ++j ) {
						// 	vkDestroyBuffer(device, mainRenderScenePipeline.descriptors[m].uniformBuffers[j], nullptr);
						// 	vkFreeMemory(device, mainRenderScenePipeline.descriptors[m].uniformBuffersMemory[j], nullptr);
						// }
					}
				textureImages.clear();
		// 	}
		// }
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

		clearPipeline( directionalLightTextureImages );
		clearPipeline( pointLightTextureImages );
		clearPipeline( spotLightTextureImages );

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void CVulkanRenderer::cleanup() {
        cleanupSwapChain();

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
		for ( size_t j = 0; j < fontIndicesContainer.size(); ++j ) {
			vkDestroyBuffer(device, fontVertexBufferContainer[fontIndicesContainer[j]], nullptr);
			vkFreeMemory(device, fontVertexBufferMemoryContainer[fontIndicesContainer[j]], nullptr);
		}
		for ( size_t j = 0; j < fontIndicesContainer.size(); ++j ) {
			vkDestroyBuffer(device, fontIndexBufferContainer[fontIndicesContainer[j]], nullptr);
			vkFreeMemory(device, fontIndexBufferMemoryContaner[fontIndicesContainer[j]], nullptr);
		}
		vkDestroyBuffer(device, modelMatrixUniformBuffer, nullptr);
		vkFreeMemory(device, modelMatrixUniformBuffersMemory, nullptr);
		vkDestroyBuffer(device, lightDataUniformBuffer, nullptr);
		vkFreeMemory(device, lightDataUniformBuffersMemory, nullptr);

		vkDeviceWaitIdle(device);

		for( int i = 0; i < SpecificPipeline::PIPELINES_NUMBER; ++i ) {
			vkDestroyRenderPass( device, renderPasses[i], nullptr );
		}
		
		for ( unsigned int i = 0; i < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++i ) {
			vkDestroyDescriptorSetLayout(device, descriptorSetsConfig[i].setLayout, nullptr);
		}
		for ( unsigned int i = 0; i < SpecificPipeline::PIPELINES_NUMBER; ++i ) {
			vkDestroyPipeline(device, pipelineConfigs[i].pipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelineConfigs[i].pipelineLayout, nullptr);
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

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroySemaphore(device, directionalLightShadowMapImageAvailableSemaphores[i], nullptr);
//            vkDestroySemaphore(device, directionalLightShadowMapRenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, directionalLightShadowMapInFlightFences[i], nullptr);

			vkDestroySemaphore(device, spotLightShadowMapImageAvailableSemaphores[i], nullptr);
//            vkDestroySemaphore(device, spotLightShadowMapRenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, spotLightShadowMapInFlightFences[i], nullptr);

			vkDestroySemaphore(device, pointLightShadowMapImageAvailableSemaphores[i], nullptr);
//            vkDestroySemaphore(device, pointLightShadowMapRenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, pointLightShadowMapInFlightFences[i], nullptr);
			
//            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);

			vkDestroySemaphore(device, hudImageAvailableSemaphores[i], nullptr);
//            vkDestroySemaphore(device, hudRenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, hudInFlightFences[i], nullptr);

			vkDestroySemaphore(device, fontImageAvailableSemaphores[i], nullptr);
//            vkDestroySemaphore(device, fontRenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, fontInFlightFences[i], nullptr);

			vkDestroySemaphore(device, virtualTexturesImageAvailableSemaphores[i], nullptr);
//            vkDestroySemaphore(device, virtualTexturesRenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, virtualTexturesInFlightFences[i], nullptr);
        }

		for( size_t i = 0; i < swapChainImages.size(); ++i ) {
			vkDestroySemaphore(device, directionalLightShadowMapRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, spotLightShadowMapRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, pointLightShadowMapRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, hudRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, fontRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, virtualTexturesRenderFinishedSemaphores[i], nullptr);
		}
		
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
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		vkDeviceWaitIdle(device);
        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
        Window->Close();
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
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        if (vkCreateWaylandSurfaceKHR(instance, &createWaylandSurfaceInfo, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
        if (vkCreateXlibSurfaceKHR(instance, &createXlibSurfaceInfo, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
        if (vkCreateXcbSurfaceKHR(instance, &createXcbSurfaceInfo, nullptr, &surface) != VK_SUCCESS) {
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
			VkPhysicalDeviceProperties prop;
			vkGetPhysicalDeviceProperties(device, &prop);
			
            if (isDeviceSuitable(device)) {
				// std::cout << prop.deviceType << std::endl;
				// std::cout << prop.deviceName << std::endl;
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
        VkExtent2D  extent = chooseSwapExtent(swapChainSupport.capabilities);

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
			VK_Image swapChainImage		 = {
				.image				 = swapChainImages[i],
				.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
				.aspectFlags         = VK_IMAGE_ASPECT_COLOR_BIT,
				.format				 = swapChainImageFormat,
				.red                 = VK_COMPONENT_SWIZZLE_IDENTITY,
				.green               = VK_COMPONENT_SWIZZLE_IDENTITY,
				.blue                = VK_COMPONENT_SWIZZLE_IDENTITY,
				.alpha               = VK_COMPONENT_SWIZZLE_IDENTITY,
				.arrayLayers         = 1,
				.width               = swapChainExtent.width,
				.height              = swapChainExtent.height
			};

            swapChainImageViews[i] = createImageView(swapChainImage, 0, 1);
        }
    }

    void CVulkanRenderer::createMainRenderPass() {
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::MAIN_RENDER_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void CVulkanRenderer::createFontRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::FONT_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
	
    void CVulkanRenderer::createHudRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::HUD_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
	
    void CVulkanRenderer::createHudScreenRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::HUD_SCREEN_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void CVulkanRenderer::createRenderPass_UI() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::UI_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void CVulkanRenderer::createRenderPassIcons_UI() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::UI_ICONS_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
	
    void CVulkanRenderer::createDirectionalLightShadowMapRenderPass() {
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void CVulkanRenderer::createSpotLightShadowMapRenderPass() {
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::SPOT_LIGHT_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void CVulkanRenderer::createPointLightShadowMapRenderPass() {
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

		std::array<VkAttachmentDescription, 1> attachments = {attachmentDescription};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = dependencies;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::POINT_LIGHT_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void CVulkanRenderer::createVirtualTextureRenderPass() {
		VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPasses[SpecificPipeline::VIRTUAL_TEXTURES_PIPELINE]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
	
    void CVulkanRenderer::createDescriptorSetLayout(DescriptorSet& descriptorSet) {
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		std::cout << "NEXT DS" << std::endl;
		std::cout << "binding count: " << descriptorSet.actualLinkedDescriptorBindingsNumber << std::endl;
		for ( u32 j = 0; j < descriptorSet.actualLinkedDescriptorBindingsNumber; ++j ) {
			u32 currentDescriptorBindingID = descriptorSet.descriptorsBindingsIDs[j];
//			u32 currentDescriptorBindingID = j;
			std::cout << "DS ID: " << currentDescriptorBindingID << std::endl;
			VkDescriptorSetLayoutBinding modelMatrixUboLayout{};
			modelMatrixUboLayout.binding = descriptorBindingsConfig[currentDescriptorBindingID].binding;
			modelMatrixUboLayout.descriptorCount = descriptorBindingsConfig[currentDescriptorBindingID].shaderDescriptorsNumber;
			modelMatrixUboLayout.descriptorType = descriptorBindingsConfig[currentDescriptorBindingID].vkType;
			modelMatrixUboLayout.pImmutableSamplers = nullptr;
			modelMatrixUboLayout.stageFlags = descriptorBindingsConfig[currentDescriptorBindingID].shaderStageFlag;

			bindings.push_back(modelMatrixUboLayout);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.flags = 0;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		std::cout << "NUMBER OF BINDINGS: " << static_cast<uint32_t>(bindings.size()) << std::endl;
		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSet.setLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
    }

    void CVulkanRenderer::createGraphicsPipeline(Pipeline& pipeline, VkRenderPass renderPass, VkPolygonMode polygonMode) {
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
        rasterizer.polygonMode = polygonMode;
        rasterizer.lineWidth = 1.0f;
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

		/*
		  Need to access inside pipeline and take ID for specific descriptor set,
		  then with that ID we got descriptor set and take it's layout
		 */
		unsigned int descriptorLayoutsNumber = pipeline.actualLinkedDescriptorSetsNumber;
		core::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		for ( unsigned i = 0; i < descriptorLayoutsNumber; ++i ) {
			std::cout << "ds inside pipeline: " << pipeline.linkedDescriptorSetIDs[i] << std::endl;
			descriptorSetLayouts.Push( descriptorSetsConfig[pipeline.linkedDescriptorSetIDs[i]].setLayout );
		}
		
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = descriptorLayoutsNumber;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.GetVectorContainer();

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

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

		if (pipeline.vertShader != nullptr)
			vkDestroyShaderModule(device, vertShaderModule, nullptr);

		if (pipeline.fragShader != nullptr)
			vkDestroyShaderModule(device, fragShaderModule, nullptr);
    }

    void CVulkanRenderer::createFramebuffers() {
		/// Main renderer frame buffers initialization
		swapChainFramebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
			std::vector<VkImageView> mainRenderAttachments;
			mainRenderAttachments.push_back(swapChainImageViews[i]);
			mainRenderAttachments.push_back(mainDepthImageView);

			createRenderPassFramebuffers(mainRenderAttachments, renderPasses[SpecificPipeline::MAIN_RENDER_PIPELINE], swapChainFramebuffers[i],
										 swapChainExtent.width, swapChainExtent.height);
		}

		/// Directional lights shadow map renderer frame buffers initialization
		unsigned int directionalLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[1];		
		directionalLightShadowMapFrameBuffers.resize(DIRECTIONAL_LIGHTS_NUMBER);
        for (size_t i = 0; i < DIRECTIONAL_LIGHTS_NUMBER; ++i) {
			std::vector<VkImageView> directionalLightsRenderAttachments;
			directionalLightsRenderAttachments.push_back((*GPUDescriptors[descriptorBindingsConfig[directionalLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage).views[0]);
			createRenderPassFramebuffers(directionalLightsRenderAttachments, renderPasses[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE],
										 directionalLightShadowMapFrameBuffers[i], swapChainExtent.width, swapChainExtent.height);
		}

		/// Spot lights shadow map renderer frame buffers initialization
		unsigned int spotLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[3];
		spotLightShadowMapFrameBuffers.resize(SPOT_LIGHTS_NUMBER);
        for (size_t i = 0; i < SPOT_LIGHTS_NUMBER; ++i) {
			std::vector<VkImageView> spotLightsRenderAttachments;
			spotLightsRenderAttachments.push_back((*GPUDescriptors[descriptorBindingsConfig[spotLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage).views[0]);
			
			createRenderPassFramebuffers(spotLightsRenderAttachments, renderPasses[SpecificPipeline::SPOT_LIGHT_PIPELINE],
										 spotLightShadowMapFrameBuffers[i], swapChainExtent.width, swapChainExtent.height);
		}

		/// Point lights shadow map renderer frame buffers initialization
		unsigned int descriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[2];
		pointLightShadowMapFrameBuffers.resize(POINT_LIGHTS_NUMBER);
		for ( size_t j = 0; j < POINT_LIGHTS_NUMBER; ++j ) {
			for ( size_t m = 0; m < 6; ++m ) {
				std::vector<VkImageView> pointLightsRenderAttachments;
				pointLightsRenderAttachments.push_back((*GPUDescriptors[descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + j].GPUImage).views[m]);
				pointLightShadowMapFrameBuffers[j].push_back({});
				createRenderPassFramebuffers(pointLightsRenderAttachments, renderPasses[SpecificPipeline::POINT_LIGHT_PIPELINE],
											 pointLightShadowMapFrameBuffers[j][m], SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
			}
		}
    }

    void CVulkanRenderer::createRenderPassFramebuffers(std::vector<VkImageView>& attachments, VkRenderPass& renderPass_, VkFramebuffer& swapChainFramebuffer, uint32_t width, uint32_t height) {
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass_;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;
			
            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
    }
	
    void CVulkanRenderer::createCommandPool(VkCommandPool& commandPool) {
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

		VK_Image depthImage		 = {
			.image				 = VkImage{},
			.deviceMemory		 = VkDeviceMemory{},
			.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
			.createFlags		 = 0,
			.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			.usageFlags			 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.aspectFlags         = VK_IMAGE_ASPECT_DEPTH_BIT,
			.format				 = depthFormat,
			.tiling				 = VK_IMAGE_TILING_OPTIMAL,
			.arrayLayers		 = 1,
			.width				 = swapChainExtent.width,
			.height				 = swapChainExtent.height,
		};

		createImage(depthImage);
		mainDepthPipelineImage = depthImage.image;
		mainDepthPipelineImageMemory = depthImage.deviceMemory;
        mainDepthImageView = createImageView(depthImage, 0, 1);
    }

	void CVulkanRenderer::createDirectionalLightShadowMapDepthResources() {
		unsigned int directionalLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[1];		
		for ( unsigned int i = 0; i < DIRECTIONAL_LIGHTS_NUMBER; ++i ) {
			VK_Image depthImage		 = {
				.image				 = VkImage{},
				.deviceMemory		 = VkDeviceMemory{},
				.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
				.createFlags		 = 0,
				.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				.usageFlags			 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.aspectFlags         = VK_IMAGE_ASPECT_DEPTH_BIT,
				.format				 = findDepthFormat(),
				.tiling				 = VK_IMAGE_TILING_OPTIMAL,
				.arrayLayers		 = 1,
				.width				 = swapChainExtent.width,
				.height				 = swapChainExtent.height,
			};

			createImage(depthImage);
			
			VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = depthImage.image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
				);

			endSingleTimeCommands(mainRenderCommandPool, commandBuffer);
			
			depthImage.views.push_back(createImageView(depthImage, 0, 1));
			setImageDebugObjectName(depthImage);
			*GPUDescriptors[descriptorBindingsConfig[directionalLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage = depthImage;
		}
	}

	void CVulkanRenderer::createSpotLightShadowMapDepthResources() {
		unsigned int spotLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[3];		
		for ( unsigned int i = 0; i < SPOT_LIGHTS_NUMBER; ++i ) {
			VK_Image depthImage		 = {
				.image				 = VkImage{},
				.deviceMemory		 = VkDeviceMemory{},
				.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
				.createFlags		 = 0,
				.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				.usageFlags			 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.aspectFlags         = VK_IMAGE_ASPECT_DEPTH_BIT,
				.format				 = findDepthFormat(),
				.tiling				 = VK_IMAGE_TILING_OPTIMAL,
				.arrayLayers		 = 1,
				.width				 = swapChainExtent.width,
				.height				 = swapChainExtent.height,
			};

			createImage(depthImage);

			VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = depthImage.image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
				);

			endSingleTimeCommands(mainRenderCommandPool, commandBuffer);
			
			depthImage.views.push_back(createImageView(depthImage, 0, 1));
			setImageDebugObjectName(depthImage);
			*GPUDescriptors[descriptorBindingsConfig[spotLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage = depthImage;
		}
	}

	void CVulkanRenderer::createPointLightShadowMapDepthResources() {
		unsigned int descriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[2];
		for ( unsigned int i = 0; i < POINT_LIGHTS_NUMBER; ++i ) {
			VK_Image depthImage		 = {
				.image				 = VkImage{},
				.deviceMemory		 = VkDeviceMemory{},
				.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
				.createFlags		 = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				.usageFlags			 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.aspectFlags         = VK_IMAGE_ASPECT_DEPTH_BIT,
				.format				 = findDepthFormat(),
				.tiling				 = VK_IMAGE_TILING_OPTIMAL,
				.arrayLayers		 = 6,
				.width				 = SHADOW_MAP_SIZE,
				.height				 = SHADOW_MAP_SIZE
			};
			
			createImage(depthImage);

			for ( unsigned int j = 0; j < 6; ++j ) {
				VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
//				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = depthImage.image;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				barrier.subresourceRange.baseMipLevel = 0;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 6;

				VkPipelineStageFlags sourceStage;
				VkPipelineStageFlags destinationStage;

				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = 0;

				sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

				vkCmdPipelineBarrier(
					commandBuffer,
					sourceStage, destinationStage,
					0,
					0, nullptr,
					0, nullptr,
					1, &barrier
					);

				endSingleTimeCommands(mainRenderCommandPool, commandBuffer);

				
				depthImage.views.push_back(createImageView(depthImage, j, 1));
			}

			setImageDebugObjectName(depthImage);
			std::cout << "GPU DESCRIPTORS: " << GPUDescriptors.GetSize() << std::endl;
			std::cout << "OFFSET: " << descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i << std::endl;
			*GPUDescriptors[descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage = depthImage;
		}

		for ( unsigned int i = 0; i < POINT_LIGHTS_NUMBER; ++i ) {
		    (*GPUDescriptors[descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage).viewType = VK_IMAGE_VIEW_TYPE_CUBE;

			setImageDebugObjectName(*GPUDescriptors[descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage);
			(*GPUDescriptors[descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage).views.push_back(
				createImageView(*GPUDescriptors[descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage, 0, 6));
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
            textureImages[i].views.push_back(createImageView(textureImages[i], 0, 1));
    }

    void CVulkanRenderer::createTextureSampler() {
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

		textureSampler = {};              /// TODO: Is it realy need here?
		if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
    }

    VkImageView CVulkanRenderer::createImageView(VK_Image image, uint32_t baseArrayLayers, uint32_t layerCount) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image.image;
        viewInfo.viewType = image.viewType;
        viewInfo.format = image.format;
		viewInfo.components.r = image.red;
		viewInfo.components.g = image.green;
		viewInfo.components.b = image.blue;
		viewInfo.components.a = image.alpha;
        viewInfo.subresourceRange.aspectMask = image.aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = baseArrayLayers;
        viewInfo.subresourceRange.layerCount = layerCount;

        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    void CVulkanRenderer::createImage(VK_Image& image) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = image.width;
        imageInfo.extent.height = image.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = image.arrayLayers;
        imageInfo.format = image.format;
        imageInfo.tiling = image.tiling;
        imageInfo.usage = image.usageFlags;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = image.createFlags;

        if (vkCreateImage(device, &imageInfo, nullptr, &image.image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image.image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, image.memoryPropertyFlags);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &image.deviceMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image.image, image.deviceMemory, 0);
    }

    void CVulkanRenderer::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

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

        endSingleTimeCommands(mainRenderCommandPool, commandBuffer);
    }

    void CVulkanRenderer::transitionShadowMapImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(directionalLightCommandPool);

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

        endSingleTimeCommands(directionalLightCommandPool, commandBuffer);
    }
	
    void CVulkanRenderer::copyBufferToImage(VkBuffer& buffer, VkImage image, uint32_t width, uint32_t height) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

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

        endSingleTimeCommands(mainRenderCommandPool, commandBuffer);
    }

    void CVulkanRenderer::createVertexBuffer(VkBuffer& _vertexBuffer, VkDeviceMemory& _vertexBufferMemory, core::vector<Vertex>& _vertices) {
        VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.GetSize();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, _vertices.GetVectorContainer(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

        copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void CVulkanRenderer::createIndexBuffer(VkBuffer& _indexBuffer, VkDeviceMemory& _indexBufferMemory, const std::vector<uint32_t>& _indices) {
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

	// void CVulkanRenderer::createMemoryArenaBuffers(VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize,
	// 	) {
		
	// }
	
    void CVulkanRenderer::createMainRenderUniformBuffers() {
        VkDeviceSize modelMatrixBufferSize = sizeof(ModelMatrixUBO);
		VkDeviceSize lightDataBufferSize = sizeof(LightData);
		VkDeviceSize modelShadowMapMatrixBufferSize = sizeof(ShadowMapMatrixUBO);
		VkDeviceSize modelCubeShadowMapMatrixBufferSize = sizeof(PointLightShadowMapMatrixUBO);
		VkDeviceSize fontUboSize = sizeof(FONT_UBO);
		VkDeviceSize hudScreenUboSize = sizeof(HUD_SCREEN_UBO);
		VkDeviceSize uiUboSize = sizeof(UI_UBO);
		VkDeviceSize uiIconsUboSize = sizeof(UI_UBO);
		VkDeviceSize hudBufferSize = sizeof(HUD_UBO);
		VkDeviceSize virtualTexturesBufferSize = sizeof(VIRTUAL_TEXTURES_UBO);
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager   = ecs::ComponentManager::GetInstance();

		core::vector<Entity> matrixLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							cm::material,
																							cm::mesh>();

		u32 memory = 0;
		memory = modelMatrixBufferSize * MAX_FRAMES_IN_FLIGHT * matrixUboDescriptorsNumber;
		unsigned int mainMatrixUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_MATRIX_UBO].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[mainMatrixUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[mainMatrixUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

		memory = hudBufferSize * MAX_FRAMES_IN_FLIGHT * hudUboDescriptorNumber;
		unsigned int hudUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::HUD].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[hudUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[hudUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);


		memory = fontUboSize * MAX_FRAMES_IN_FLIGHT * fontUboDescriptorNumber * 8;
		unsigned int fontUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::FONT_RENDER_UBO].descriptorsBindingsIDs[0];
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[fontUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[fontUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

		memory = hudScreenUboSize * MAX_FRAMES_IN_FLIGHT * hudScreenUboDescriptorNumber;
		unsigned int hudScreenUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::HUD_SCREEN].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

		memory = uiUboSize * MAX_FRAMES_IN_FLIGHT * uiUboDescriptorsNumber;
		unsigned int uiUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::UI].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[uiUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[uiUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

		memory = uiIconsUboSize * MAX_FRAMES_IN_FLIGHT * uiIconsDescriptorsNumber;
		unsigned int uiIconsUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::UI_ICONS].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[uiIconsUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[uiIconsUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
		
		memory = modelShadowMapMatrixBufferSize * MAX_FRAMES_IN_FLIGHT * directionalLightUboDescriptorsNumber;
		unsigned int shadowMapDirectionalLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_DIRECTIONAL_LIGHT].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[shadowMapDirectionalLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[shadowMapDirectionalLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

		memory = modelShadowMapMatrixBufferSize * MAX_FRAMES_IN_FLIGHT * spotLightUboDescriptorsNumber;
		unsigned int shadowMapSpotLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_SPOT_LIGHT].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[shadowMapSpotLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[shadowMapSpotLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
		
		memory = modelCubeShadowMapMatrixBufferSize * MAX_FRAMES_IN_FLIGHT * pointLightUboDescriptorsNumber;
		unsigned int shadowMapPointLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_POINT_LIGHT].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[shadowMapPointLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[shadowMapPointLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

		memory = lightDataBufferSize * MAX_FRAMES_IN_FLIGHT;
		unsigned int lightDataUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[0];		
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[lightDataUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[lightDataUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

		memory = virtualTexturesBufferSize * MAX_FRAMES_IN_FLIGHT;
		unsigned int virtualTexturesUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::VIRTUAL_TEXTURES_UBO].descriptorsBindingsIDs[0];
		createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 GPUDescriptors[descriptorBindingsConfig[virtualTexturesUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
					 GPUDescriptors[descriptorBindingsConfig[virtualTexturesUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
    }

    void CVulkanRenderer::createMainRenderDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};

		uint32_t descriptorCount = 320000;
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(descriptorCount);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(descriptorCount);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(descriptorCount);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

	void CVulkanRenderer::allocateDescriptorSets( core::vector<VkDescriptorSet>& descriptorSets, VkDescriptorSetLayout setLayout,
												  const unsigned int descriptorSetsNumber, const unsigned int descriptorOffset ) {
		std::vector<VkDescriptorSetLayout> matrixUboLayouts(descriptorSetsNumber, setLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetsNumber);
		allocInfo.pSetLayouts = matrixUboLayouts.data();

//		descriptorSets.Resize(descriptorSetsNumber);
		if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.GetVectorContainer() + descriptorOffset) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}

	void CVulkanRenderer::updateDescriptorSetsUBO( VkBuffer ubo, const VkDeviceSize& uboStructSize, const unsigned int& uboDescriptorsNumber,
												   int uboBinding, [[maybe_unused]] core::vector<VkDescriptorSet>& uboDescriptorSets, const unsigned int offset ) {
		for (size_t i = 0; i < uboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo = createDescriptorBufferInfo( ubo, uboStructSize, i );
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
			
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = *(descriptorSetsChunks.GetVectorContainer() + offset + i);
			descriptorWrites[0].dstBinding = uboBinding;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void CVulkanRenderer::updateLightDataDescriptorSets( const DescriptorSet& currentDescriptorSet1, const VkDeviceSize& uboStructSize ) {
		// unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[1];
		// std::cout << "INDEX SUK: " << linkedDescriptorSetID << std::endl;
		// const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetID];

		const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet1.actualLinkedDescriptorBindingsNumber;
		core::vector<u32> shaderBindings;
		core::vector<u32> descriptorNumberPerBinding;
		core::vector<u32> bindingsIDs;
		for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
			shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[j]].binding );
			descriptorNumberPerBinding.Push( descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[j]].shaderDescriptorsNumber );
			bindingsIDs.Push( currentDescriptorSet1.descriptorsBindingsIDs[j] );
		}
		// unsigned int uboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[0];		
		// unsigned int directionalLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[1];		
		// for( int i = 0; i < DIRECTIONAL_LIGHTS_NUMBER; ++i ) {
		// directionalLightsImageInfo[i] = createDescriptorImageInfo( *GPUDescriptors[descriptorBindingsConfig[directionalLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage,
		// 														   VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 0, textureSampler );
		// }
		// unsigned int pointLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[2];		
		// for( int i = 0; i < POINT_LIGHTS_NUMBER; ++i ) {
		// 	pointLightsImageInfo[i] = createDescriptorImageInfo( *GPUDescriptors[descriptorBindingsConfig[pointLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage,
		// 														 VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 6, textureSampler );
		// }
		// unsigned int spotLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[3];		
		// for( int i = 0; i < SPOT_LIGHTS_NUMBER; ++i ) {
		// 	spotLightsImageInfo[i] = createDescriptorImageInfo ( *GPUDescriptors[descriptorBindingsConfig[spotLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage,
		// 								VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 0, textureSampler );
		// }
		// std::cout << "1 bind: " << uboBinding << std::endl;
		// std::cout << "2 bind: " << shaderBindings[1] << std::endl;
		// std::cout << "3 bind: " << shaderBindings[2] << std::endl;
		// std::cout << "4 bind: " << shaderBindings[3] << std::endl;
		for ( size_t i = 0; i < currentDescriptorSet1.hostDescriptorNumber; ++i ) {
			core::vector<VkWriteDescriptorSet> descriptorWrites;
			descriptorWrites.Resize( shaderBindings.GetSize() );
//			std::cout << "NEXT HOST DS" << std::endl;
			core::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
			core::vector<core::vector<VkDescriptorImageInfo>> descriptorImageInfos;
			for( size_t j = 0; j < shaderBindings.GetSize(); ++j ) {
//				unsigned int descriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[j];
//				std::cout << "DS BINDING INDEX: " << descriptorBindingIndex << std::endl;
				if( descriptorBindingsConfig[bindingsIDs[j]].vkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {
					descriptorBufferInfos.Push({});
//					std::cout << "UBO" << std::endl;
					for( size_t m = 0; m < descriptorBindingsConfig[bindingsIDs[j]].shaderDescriptorsNumber; ++m ) {
						descriptorBufferInfos[j] = createDescriptorBufferInfo(
							GPUDescriptors[descriptorBindingsConfig[bindingsIDs[j]].globalDescriptorOffset].GPUBuffer->buffer,
							uboStructSize, i );
					}
					descriptorWrites[j].pBufferInfo = descriptorBufferInfos.GetVectorContainer();
				} else if ( descriptorBindingsConfig[bindingsIDs[j]].vkType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ) {
					descriptorImageInfos.Push({});
//					std::cout << "SAMPLER: " << std::endl;
					for( size_t m = 0; m < descriptorBindingsConfig[bindingsIDs[j]].shaderDescriptorsNumber; ++m ) {
						u32 imageViewIndex = GPUDescriptors[descriptorBindingsConfig[bindingsIDs[j]].globalDescriptorOffset + m].GPUImage->views.size() - 1;
//						std::cout << "IMAGE VIEW INDEX: " << imageViewIndex << std::endl;
						descriptorImageInfos[descriptorImageInfos.GetSize() - 1].Push({});
						descriptorImageInfos[descriptorImageInfos.GetSize() - 1][m] = createDescriptorImageInfo(
							*GPUDescriptors[descriptorBindingsConfig[bindingsIDs[j]].globalDescriptorOffset + m].GPUImage,
							VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, imageViewIndex, textureSampler );
					}
					descriptorWrites[j].pImageInfo = descriptorImageInfos[descriptorImageInfos.GetSize() - 1].GetVectorContainer();
				}
				
				descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[j].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + i);
				descriptorWrites[j].dstBinding = shaderBindings[j];
				descriptorWrites[j].dstArrayElement = 0;
				descriptorWrites[j].descriptorType = descriptorBindingsConfig[bindingsIDs[j]].vkType;
				descriptorWrites[j].descriptorCount = descriptorBindingsConfig[bindingsIDs[j]].shaderDescriptorsNumber;

			}
			// VkDescriptorBufferInfo modelMatrixBufferInfo{};
			// modelMatrixBufferInfo.buffer = GPUDescriptors[descriptorBindingsConfig[uboDescriptorBindingIndex].globalDescriptorOffset + i].GPUBuffer->buffer;
			// modelMatrixBufferInfo.offset = i * uboStructSize;
			// modelMatrixBufferInfo.range = uboStructSize;

			// std::cout << "buffer offset: " << modelMatrixBufferInfo.offset << std::endl;
			// std::cout << "buffer size: " << uboStructSize << std::endl;
			// std::cout << "buffer range: " << modelMatrixBufferInfo.range << std::endl;
			

			

			// descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// descriptorWrites[1].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + i);
			// descriptorWrites[1].dstBinding = shaderBindings[1];
			// descriptorWrites[1].dstArrayElement = 0;
			// descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			// descriptorWrites[1].descriptorCount = DIRECTIONAL_LIGHTS_NUMBER;
			// descriptorWrites[1].pImageInfo = directionalLightsImageInfo;

			// descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// descriptorWrites[2].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + i);
			// descriptorWrites[2].dstBinding = shaderBindings[2];
			// descriptorWrites[2].dstArrayElement = 0;
			// descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			// descriptorWrites[2].descriptorCount = POINT_LIGHTS_NUMBER;
			// descriptorWrites[2].pImageInfo = pointLightsImageInfo;

			// descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// descriptorWrites[3].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + i);
			// descriptorWrites[3].dstBinding = shaderBindings[3];
			// descriptorWrites[3].dstArrayElement = 0;
			// descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			// descriptorWrites[3].descriptorCount = SPOT_LIGHTS_NUMBER;
			// descriptorWrites[3].pImageInfo = spotLightsImageInfo;
			
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
		}
	}
	
	void CVulkanRenderer::updateDescriptorSetsCombinedImageSampler( const DescriptorSet& descriptorSet ) {
		core::vector<u32> bindingsIDs;
		for ( size_t j = 0; j < descriptorSet.actualLinkedDescriptorBindingsNumber; ++j ) {
			bindingsIDs.Push( descriptorSet.descriptorsBindingsIDs[j] );
		}
		
		for (size_t i = 0; i < descriptorSet.hostDescriptorNumber; ++i) {
			const unsigned int textureIndex = i / 2;
			constexpr unsigned int textureViewIndex = 0;
			VkDescriptorImageInfo imageInfo = createDescriptorImageInfo( textureImages[textureIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureViewIndex, textureSampler );
			core::vector<VkWriteDescriptorSet> descriptorWrites{};

			for ( unsigned int j = 0; j < bindingsIDs.GetSize(); ++j ) {
				descriptorWrites.Push({});
				const unsigned int lastElement = descriptorWrites.GetSize() - 1;
				descriptorWrites[lastElement].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[lastElement].dstSet = *(descriptorSetsChunks.GetVectorContainer() + descriptorSet.descriptorSetOffset + i);
				descriptorWrites[lastElement].dstBinding = descriptorBindingsConfig[bindingsIDs[j]].binding;;
				descriptorWrites[lastElement].dstArrayElement = 0;
				descriptorWrites[lastElement].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[lastElement].descriptorCount = 1;
				descriptorWrites[lastElement].pImageInfo = &imageInfo;
			}
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
		}
	}
	
	void CVulkanRenderer::createDescriptorImageInfo( const unsigned int descriptorNumber, VkImageLayout imageLayout,
													 core::vector<VK_Image>& textureImages, const unsigned int imageViewIndex,
													 VkDescriptorImageInfo descriptorImageInfos[] ) {
		for (size_t i = 0; i < descriptorNumber; ++i) {
			descriptorImageInfos[i] = {};
			descriptorImageInfos[i].imageLayout = imageLayout;
			descriptorImageInfos[i].imageView = textureImages[i].views[imageViewIndex];
			descriptorImageInfos[i].sampler = textureSampler;
		}
	}
	
	void CVulkanRenderer::updateDescriptorSets( [[maybe_unused]] std::vector<VkDescriptorSet>& descriptorSets, [[maybe_unused]] const Pipeline& pipeline,
												[[maybe_unused]] DescriptorsTypes descriptorType) {
	}

	VkDescriptorBufferInfo CVulkanRenderer::createDescriptorBufferInfo( VkBuffer ubo, u32 offset, u32 range ) {
		VkDescriptorBufferInfo uboInfo{};
		uboInfo.buffer = ubo;
		uboInfo.offset = offset;
		uboInfo.range = range;

		return uboInfo;
	}
	
    void CVulkanRenderer::createDirectionalLightShadowMapDescriptorSets() {
		const unsigned int linkedDescriptorSetsNumber = pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].actualLinkedDescriptorSetsNumber;
		for( size_t descriptorSetID = 0; descriptorSetID < linkedDescriptorSetsNumber; ++descriptorSetID ) {
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].linkedDescriptorSetIDs[descriptorSetID];
			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet.setLayout,
									currentDescriptorSet.hostDescriptorNumber, currentDescriptorSet.descriptorSetOffset );


			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[j]].binding );
			}
			for (size_t i = 0; i < currentDescriptorSet.hostDescriptorNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites;
				unsigned int shadowMapDirectionalLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_DIRECTIONAL_LIGHT].descriptorsBindingsIDs[0];		
				VkDescriptorBufferInfo modelMatrixBufferInfo = createDescriptorBufferInfo(
					GPUDescriptors[descriptorBindingsConfig[shadowMapDirectionalLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer, sizeof( ShadowMapMatrixUBO ), i );			   

				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					
					descriptorWrites.Push({});
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + i);
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}

    void CVulkanRenderer::createSpotLightShadowMapDescriptorSets() {
		const unsigned int linkedDescriptorSetsNumber = pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].actualLinkedDescriptorSetsNumber;
		for( size_t descriptorSetID = 0; descriptorSetID < linkedDescriptorSetsNumber; ++descriptorSetID ) {
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].linkedDescriptorSetIDs[descriptorSetID];
 			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet.setLayout,
									currentDescriptorSet.hostDescriptorNumber, currentDescriptorSet.descriptorSetOffset );

			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[j]].binding );
			}
			
			for (size_t i = 0; i < spotLightUboDescriptorsNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites{};
				VkDescriptorBufferInfo modelMatrixBufferInfo{};
				unsigned int shadowMapSpotLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_SPOT_LIGHT].descriptorsBindingsIDs[0];		
				modelMatrixBufferInfo.buffer = GPUDescriptors[descriptorBindingsConfig[shadowMapSpotLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
				modelMatrixBufferInfo.offset = i * sizeof(ShadowMapMatrixUBO);
				modelMatrixBufferInfo.range = sizeof(ShadowMapMatrixUBO);

				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					descriptorWrites.Push({});
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + i);;
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}

	void CVulkanRenderer::createPointLightShadowMapDescriptorSets() {
		const unsigned int linkedDescriptorSetsNumber = pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].actualLinkedDescriptorSetsNumber;
		for( size_t descriptorSetID = 0; descriptorSetID < linkedDescriptorSetsNumber; ++descriptorSetID ) {
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].linkedDescriptorSetIDs[descriptorSetID];
 			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID]; 
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet.setLayout,
									currentDescriptorSet.hostDescriptorNumber, currentDescriptorSet.descriptorSetOffset );

			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[j]].binding );
			}
//			std::cout << "SIZE OF DS: " << descriptorSetsChunks.GetSize() << std::endl;
			for (size_t i = 0; i < currentDescriptorSet.hostDescriptorNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites{};
				VkDescriptorBufferInfo modelMatrixBufferInfo{};
				unsigned int shadowMapPointLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_POINT_LIGHT].descriptorsBindingsIDs[0];		
				modelMatrixBufferInfo.buffer = GPUDescriptors[descriptorBindingsConfig[shadowMapPointLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
				modelMatrixBufferInfo.offset = i * sizeof(PointLightShadowMapMatrixUBO);
				modelMatrixBufferInfo.range = sizeof(PointLightShadowMapMatrixUBO);
				
				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					descriptorWrites.Push({});
 					// std::cout << "offset: " << currentDescriptorSet.descriptorSetOffset << std::endl;
					// std::cout << "i: " << i << std::endl;
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + i);;
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}

    void CVulkanRenderer::createHudDescriptorSets() {
		const unsigned int linkedDescriptorSetsNumber = pipelineConfigs[SpecificPipeline::HUD_PIPELINE].actualLinkedDescriptorSetsNumber;
		for( size_t descriptorSetID = 0; descriptorSetID < linkedDescriptorSetsNumber; ++descriptorSetID ) {
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::HUD_PIPELINE].linkedDescriptorSetIDs[descriptorSetID];
  			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet.setLayout,
									currentDescriptorSet.hostDescriptorNumber, currentDescriptorSet.descriptorSetOffset );

			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[j]].binding );
			}
			
			for (size_t i = 0; i < currentDescriptorSet.hostDescriptorNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites{};
				VkDescriptorBufferInfo modelMatrixBufferInfo{};
				unsigned int hudUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::HUD].descriptorsBindingsIDs[0];		
				modelMatrixBufferInfo.buffer = GPUDescriptors[descriptorBindingsConfig[hudUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
				modelMatrixBufferInfo.offset = i * sizeof(HUD_UBO);
				modelMatrixBufferInfo.range = sizeof(HUD_UBO);
			
				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					descriptorWrites.Push({});
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + i);;
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}

    void CVulkanRenderer::createHudScreenDescriptorSets() {
		const unsigned int linkedDescriptorSetsNumber = pipelineConfigs[SpecificPipeline::HUD_SCREEN_PIPELINE].actualLinkedDescriptorSetsNumber;
		for( size_t descriptorSetID = 0; descriptorSetID < linkedDescriptorSetsNumber; ++descriptorSetID ) {
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::HUD_SCREEN_PIPELINE].linkedDescriptorSetIDs[descriptorSetID];
   			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet.setLayout,
									currentDescriptorSet.hostDescriptorNumber, currentDescriptorSet.descriptorSetOffset );

			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[j]].binding );
			}
			
			for (size_t i = 0; i < currentDescriptorSet.hostDescriptorNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites{};
				VkDescriptorBufferInfo modelMatrixBufferInfo{};
				unsigned int hudScreenUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::HUD_SCREEN].descriptorsBindingsIDs[0];		
				modelMatrixBufferInfo.buffer = GPUDescriptors[descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
				modelMatrixBufferInfo.offset = i * sizeof(HUD_SCREEN_UBO);
				modelMatrixBufferInfo.range = sizeof(HUD_SCREEN_UBO);

				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					descriptorWrites.Push({});
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + i);;
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}

    void CVulkanRenderer::createDescriptorSets_UI() {
		const unsigned int linkedDescriptorSetUboID = pipelineConfigs[SpecificPipeline::UI_PIPELINE].linkedDescriptorSetIDs[0];
		const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetUboID];
		allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet.setLayout,
								currentDescriptorSet.hostDescriptorNumber, currentDescriptorSet.descriptorSetOffset );

		const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet.actualLinkedDescriptorBindingsNumber;
		core::vector<u32> shaderBindings;
		for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
			shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[j]].binding );
		}
		
		for (size_t i = 0; i < currentDescriptorSet.hostDescriptorNumber; ++i) {
			core::vector<VkWriteDescriptorSet> descriptorWrites{};
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			unsigned int uiUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::UI].descriptorsBindingsIDs[0];
			modelMatrixBufferInfo.buffer = GPUDescriptors[descriptorBindingsConfig[uiUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
			modelMatrixBufferInfo.offset = i * sizeof(UI_UBO);
			modelMatrixBufferInfo.range = sizeof(UI_UBO);

			for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
				descriptorWrites.Push({});
				descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + i);;
				descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
				descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
				descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
				descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
				descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
			}
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
		}

		const unsigned int linkedDescriptorSetSamplerID = pipelineConfigs[SpecificPipeline::UI_PIPELINE].linkedDescriptorSetIDs[1];
		const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetSamplerID];
		if ( initializeTextureData_.size() > 0 ) {
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet1.setLayout,
									currentDescriptorSet1.hostDescriptorNumber, currentDescriptorSet1.descriptorSetOffset );

			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet1.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[j]].binding );
			}
			
			for (size_t i = 0; i < currentDescriptorSet1.hostDescriptorNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites{};
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//				unsigned int textureIndex = i / 2;
				imageInfo.imageView = textureImages[7].views[0];
				imageInfo.sampler = textureSampler;
				
				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					descriptorWrites.Push({});
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + i);;
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pImageInfo = &imageInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}

    void CVulkanRenderer::createDescriptorSetsIcons_UI() {
		const unsigned int linkedDescriptorSetUboID = pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].linkedDescriptorSetIDs[0];
		const DescriptorSet& currentDescriptorSet0 = descriptorSetsConfig[linkedDescriptorSetUboID];
		allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet0.setLayout,
								currentDescriptorSet0.hostDescriptorNumber, currentDescriptorSet0.descriptorSetOffset );

		const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet0.actualLinkedDescriptorBindingsNumber;
		core::vector<u32> shaderBindings;
		for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
			shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[j]].binding );
		}
		
		for (size_t i = 0; i < currentDescriptorSet0.hostDescriptorNumber; ++i) {
			core::vector<VkWriteDescriptorSet> descriptorWrites{};
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			unsigned int uiIconsUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::UI_ICONS].descriptorsBindingsIDs[0];		
			modelMatrixBufferInfo.buffer = GPUDescriptors[descriptorBindingsConfig[uiIconsUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
			modelMatrixBufferInfo.offset = i * sizeof(UI_UBO);
			modelMatrixBufferInfo.range = sizeof(UI_UBO);

			for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
				descriptorWrites.Push({});
				descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet0.descriptorSetOffset + i);;
				descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
				descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
				descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
				descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
				descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
			}
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
		}

		const unsigned int linkedDescriptorSetSamplerID = pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].linkedDescriptorSetIDs[1];
		const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetSamplerID];
		if ( initializeTextureData_.size() > 0 ) {
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet1.setLayout,
									MAX_FRAMES_IN_FLIGHT * currentDescriptorSet1.hostDescriptorNumber, currentDescriptorSet1.descriptorSetOffset );

			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet1.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[j]].binding );
			}
			
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * currentDescriptorSet1.hostDescriptorNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites{};
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				unsigned int textureIndex = i / 2;
				imageInfo.imageView = textureImages[textureIndex].views[0];
				imageInfo.sampler = textureSampler;
				
				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					descriptorWrites.Push({});
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + i);;
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pImageInfo = &imageInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}
	
	void CVulkanRenderer::createFontRenderDescriptorSets() {
		const unsigned int linkedDescriptorSetUboID = pipelineConfigs[SpecificPipeline::FONT_PIPELINE].linkedDescriptorSetIDs[0];
		const DescriptorSet& currentDescriptorSet0 = descriptorSetsConfig[linkedDescriptorSetUboID];
		allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet0.setLayout,
								currentDescriptorSet0.hostDescriptorNumber, currentDescriptorSet0.descriptorSetOffset );

		const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet0.actualLinkedDescriptorBindingsNumber;
		core::vector<u32> shaderBindings;
		for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
			shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[j]].binding );
		}
		
		for  (size_t i = 0; i < currentDescriptorSet0.hostDescriptorNumber; ++i) {
			core::vector<VkWriteDescriptorSet> descriptorWrites{};
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			unsigned int fontUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::FONT_RENDER_UBO].descriptorsBindingsIDs[0];
			modelMatrixBufferInfo.buffer = GPUDescriptors[descriptorBindingsConfig[fontUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
			modelMatrixBufferInfo.offset = i * sizeof(FONT_UBO);
			modelMatrixBufferInfo.range = sizeof(FONT_UBO);

			for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
				descriptorWrites.Push({});
				descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet0.descriptorSetOffset + i);;
				descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
				descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
				descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
				descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
				descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
			}
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
		}
		
		const unsigned int linkedDescriptorSetSamplerID = pipelineConfigs[SpecificPipeline::FONT_PIPELINE].linkedDescriptorSetIDs[1];
		const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetSamplerID];
		if ( initializeTextureData_.size() > 0 ) {
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet1.setLayout,
									currentDescriptorSet1.hostDescriptorNumber, currentDescriptorSet1.descriptorSetOffset );

			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet1.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[j]].binding );
			}
			
			for (size_t i = 0; i < currentDescriptorSet1.hostDescriptorNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites{};
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = textureImages[6].views[0];
				imageInfo.sampler = textureSampler;
				
				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					descriptorWrites.Push({});
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = *(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + i);;
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pImageInfo = &imageInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}

	void CVulkanRenderer::updateFontRenderDescriptorSets() {
	}

	void CVulkanRenderer::createVirtualTexturesDescriptorSets() {
		const unsigned int linkedDescriptorSetUboID = pipelineConfigs[SpecificPipeline::VIRTUAL_TEXTURES_PIPELINE].linkedDescriptorSetIDs[0];
		const DescriptorSet& currentDescriptorSet0 = descriptorSetsConfig[linkedDescriptorSetUboID];
		allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet0.setLayout,
								MAX_FRAMES_IN_FLIGHT * currentDescriptorSet0.hostDescriptorNumber, currentDescriptorSet0.descriptorSetOffset );

		const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet0.actualLinkedDescriptorBindingsNumber;
		core::vector<u32> shaderBindings;
		for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
			shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[j]].binding );
		}
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * currentDescriptorSet0.hostDescriptorNumber; ++i) {
			core::vector<VkWriteDescriptorSet> descriptorWrites{};
			VkDescriptorBufferInfo modelMatrixBufferInfo{};
			modelMatrixBufferInfo.buffer = virtualTexturesUniformBuffer;
			modelMatrixBufferInfo.offset = i * sizeof(VIRTUAL_TEXTURES_UBO);
			modelMatrixBufferInfo.range = sizeof(VIRTUAL_TEXTURES_UBO);

			for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
				descriptorWrites.Push({});
				descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[shaderDescriptorID].dstSet = virtualTexturesUBODesctiptorSets[i];
				descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
				descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
				descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
				descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
				descriptorWrites[shaderDescriptorID].pBufferInfo = &modelMatrixBufferInfo;
			}
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
		}
		
		const unsigned int linkedDescriptorSetSamplerID = pipelineConfigs[SpecificPipeline::VIRTUAL_TEXTURES_PIPELINE].linkedDescriptorSetIDs[1];
		const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetSamplerID];
		if ( initializeTextureData_.size() > 0 ) {
			allocateDescriptorSets( virtualTexturesSamplersDesctiptorSets, currentDescriptorSet1.setLayout,
									MAX_FRAMES_IN_FLIGHT * currentDescriptorSet1.hostDescriptorNumber, currentDescriptorSet1.descriptorSetOffset );

			const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet1.actualLinkedDescriptorBindingsNumber;
			core::vector<u32> shaderBindings;
			for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
				shaderBindings.Push( descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[j]].binding );
			}
			
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * currentDescriptorSet1.hostDescriptorNumber; ++i) {
				core::vector<VkWriteDescriptorSet> descriptorWrites{};
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//				unsigned int textureIndex = i / 2;
				imageInfo.imageView = textureImages[6].views[0];
				imageInfo.sampler = textureSampler;

				for( size_t shaderDescriptorID = 0; shaderDescriptorID < shaderBindings.GetSize(); ++shaderDescriptorID ) {
					descriptorWrites.Push({});
					descriptorWrites[shaderDescriptorID].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[shaderDescriptorID].dstSet = virtualTexturesSamplersDesctiptorSets[i];
					descriptorWrites[shaderDescriptorID].dstBinding = shaderBindings[shaderDescriptorID];
					descriptorWrites[shaderDescriptorID].dstArrayElement = 0;
					descriptorWrites[shaderDescriptorID].descriptorType = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[shaderDescriptorID]].vkType;
					descriptorWrites[shaderDescriptorID].descriptorCount = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[shaderDescriptorID]].shaderDescriptorsNumber;
					descriptorWrites[shaderDescriptorID].pImageInfo = &imageInfo;
				}
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
			}
		}
	}

	void CVulkanRenderer::updateVirtualTexturesDescriptorSets() {
	}
	
    void CVulkanRenderer::createMainRenderDescriptorSets() {
		const unsigned int linkedDescriptorSetMatrixUboID = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[0];
		const DescriptorSet& currentDescriptorSet0 = descriptorSetsConfig[linkedDescriptorSetMatrixUboID];
		[[maybe_unused]] int modelMatrixUboBinding = descriptorBindingsConfig[currentDescriptorSet0.descriptorsBindingsIDs[0]].binding;
		allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet0.setLayout,
								currentDescriptorSet0.hostDescriptorNumber, currentDescriptorSet0.descriptorSetOffset );
		updateLightDataDescriptorSets( currentDescriptorSet0, sizeof(ModelMatrixUBO) );

		const unsigned int linkedDescriptorSetMatrixLightDataID = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[1];
		const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetMatrixLightDataID];
//		int lightDataUboBinding = descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[0]].binding;
		allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet1.setLayout,
								currentDescriptorSet1.hostDescriptorNumber, currentDescriptorSet1.descriptorSetOffset );

		updateLightDataDescriptorSets( currentDescriptorSet1, sizeof(LightData) );

		const unsigned int linkedDescriptorSetMatrixSpecularID = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[2];
		const DescriptorSet& currentDescriptorSet2 = descriptorSetsConfig[linkedDescriptorSetMatrixSpecularID];
		int specularSamplerBinding = descriptorBindingsConfig[currentDescriptorSet2.descriptorsBindingsIDs[0]].binding;
		const unsigned int linkedDescriptorSetMatrixDiffuseID = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[3];
		const DescriptorSet& currentDescriptorSet3 = descriptorSetsConfig[linkedDescriptorSetMatrixDiffuseID];
		int diffuseSamplerBinding = descriptorBindingsConfig[currentDescriptorSet3.descriptorsBindingsIDs[0]].binding;

		if ( initializeTextureData_.size() > 0 ) {
			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet2.setLayout,
									currentDescriptorSet2.hostDescriptorNumber, currentDescriptorSet2.descriptorSetOffset );

			core::vector<unsigned int> bindings;
			bindings.Push(specularSamplerBinding);
			updateDescriptorSetsCombinedImageSampler( currentDescriptorSet2);

			allocateDescriptorSets( descriptorSetsChunks, currentDescriptorSet3.setLayout,
									currentDescriptorSet3.hostDescriptorNumber, currentDescriptorSet3.descriptorSetOffset );

			core::vector<unsigned int> diffuseBindings;
			diffuseBindings.Push(diffuseSamplerBinding);
			updateDescriptorSetsCombinedImageSampler( currentDescriptorSet3);
		}
	}

	void CVulkanRenderer::updateSamplersDescriptroSets([[maybe_unused]] uint32_t diffuse_id, [[maybe_unused]] uint32_t specular_id) {
	}
	
	void CVulkanRenderer::updateDirectionalLightShadowMapDescriptorSets() {
	}

	void CVulkanRenderer::updateSpotLightShadowMapDescriptorSets() {
	}

	void CVulkanRenderer::updatePointLightShadowMapDescriptorSets() {
	}

    void CVulkanRenderer::updateHudDescriptorSets() {
	}
	
    void CVulkanRenderer::updateDescriptorSets() {
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

		i32 result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
        if (result != VK_SUCCESS) {
//			std::cout << "result" << result << std::endl;
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    VkCommandBuffer CVulkanRenderer::beginSingleTimeCommands(VkCommandPool& commandPool) {
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

    void CVulkanRenderer::endSingleTimeCommands(VkCommandPool& commandPool, VkCommandBuffer& commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void CVulkanRenderer::copyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(mainRenderCommandPool, commandBuffer);
    }

    uint32_t CVulkanRenderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties && memProperties.memoryTypes[i].heapIndex == 0) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void CVulkanRenderer::createCommandBuffers(VkCommandPool& commandPool, std::vector<VkCommandBuffer>& commandBuffers) {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

	void CVulkanRenderer::updateHudUBO(uint32_t offset, ecs::components::transform* entityOwnHudTransform,
									   ecs::components::health* entityOwnHudHealth, bool isHudExists,
									   float highestY) {
		HUD_UBO hudUBO{};

		hudUBO.view = viewMatrix;
		hudUBO.proj = projectionMatrix;
		
		hudUBO.isHudExists = isHudExists;
		hudUBO.currentHP   = entityOwnHudHealth->currentHealth;
		hudUBO.maxHP       = entityOwnHudHealth->maxHealth;
		hudUBO.entityPosition = entityOwnHudTransform->position;
		hudUBO.highestY    = highestY;

		void* hudMatrixData;
		unsigned int hudUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::HUD].descriptorsBindingsIDs[0];		
        vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[hudUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(HUD_UBO) * offset,
					sizeof(HUD_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(HUD_UBO));
        vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[hudUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

	void CVulkanRenderer::updateHudScreenUBO(uint32_t offset, ecs::components::transform* cursorTransform) {
		HUD_SCREEN_UBO hudUBO{};
		vec3 defaultPosition = vec3(0.0, 0.0, 0.0);

		cursorTransform->position[0] = hud_screen_x;
		cursorTransform->position[1] = -hud_screen_y;
//		std::cout << "cursor scale: " << cursorTransform->fScale << std::endl;

//		std::cout << "x: " << cursorTransform->tPosition[0] << " y: " << cursorTransform->tPosition << std::endl;
		
		if ( !isInventoryOpened ) {
			hudUBO.model[3][0] = defaultPosition[0];
			hudUBO.model[3][1] = defaultPosition[1];
			hudUBO.model[3][2] = defaultPosition[2];
			hudUBO.model[0][0] = cursorTransform->scale;
			hudUBO.model[1][1] = cursorTransform->scale;
			hudUBO.model[2][2] = cursorTransform->scale;
			hudUBO.model[3][3] = 1.0f;
		} else {
			defaultPosition[0] = hud_screen_x;
			defaultPosition[1] = -hud_screen_y;
			
			hudUBO.model[3][0] = defaultPosition[0];
			hudUBO.model[3][1] = defaultPosition[1];
			hudUBO.model[3][2] = defaultPosition[2];
			hudUBO.model[0][0] = cursorTransform->scale;
			hudUBO.model[1][1] = cursorTransform->scale;
			hudUBO.model[2][2] = cursorTransform->scale;
			hudUBO.model[3][3] = 1.0f;
		}

		void* hudMatrixData;
		unsigned int hudScreenUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::HUD_SCREEN].descriptorsBindingsIDs[0];		
        vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(HUD_SCREEN_UBO) * offset,
					sizeof(HUD_SCREEN_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(HUD_SCREEN_UBO));
        vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

	void CVulkanRenderer::updateUBO_UI(const unsigned int currentInventoryRow, const unsigned int currentInventoryColumn, uint32_t offset,
									   float slotScale, [[maybe_unused]] unsigned int inventorySlotEntity,
									   ecs::components::transform* slotTransfromComponent) {
		UI_UBO hudUBO{};
		mat4 model(1.0);
		const float fullSlotScale     = slotTransfromComponent->gltf ? slotScale * 2.0f : slotScale;
		const float x = slotTransfromComponent->position[0] + currentInventoryColumn * fullSlotScale;
		const float y_scaleMultilayer = aspectRate * fullSlotScale;
		const float y = slotTransfromComponent->position[1] + currentInventoryRow * y_scaleMultilayer;
		const float inventorySlotScale = slotScale;
		model[0][0] = inventorySlotScale;
		model[1][1] = inventorySlotScale;
		model[2][2] = inventorySlotScale;
		model[3][0] = x;
		model[3][1] = y;
		model[3][2] = 0.1f;

		// inventorySlotTransform->position[0] = x;
		// inventorySlotTransform->position[1] = y;
		// inventorySlotTransform->position[2] = 0.1f;
		
		hudUBO.model = model;

		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager = ecs::ComponentManager::GetInstance();
		core::vector<unsigned int> inventoryEntities = componentManager->collectLinkedEntities<cm::inventory>();
		cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntities[0]);

//		std::cout << "number of slots: " << inventoryComponent->highlightedSlots.GetSize() << std::endl;
		bool highLightedSlot = false;
		for ( unsigned int i = 0; i < inventoryComponent->highlightedSlots.GetSize(); ++i ) {
			if ( inventoryComponent->highlightedSlots[i] == currentInventoryRow * inventoryComponent->col + currentInventoryColumn ) {
//				std::cout << "index: " << currentInventoryRow * inventoryComponent->col + currentInventoryColumn << std::endl;
				highLightedSlot = true;
				break;
			} else
				continue;
		}

//		std::cout << "second intrence number of slots: " << inventoryComponent->highlightedSlots.GetSize() << std::endl;
		if ( inventoryComponent->highlightedSlots.GetSize() > 0 ) {
			if ( highLightedSlot ) {
				if ( inventoryComponent->isAvailableHighlightedSlots )
					hudUBO.color = { 0.0, 0.3, 0.0 };
				else
					hudUBO.color = { 0.3, 0.0, 0.0 };
			}
		} else {
			hudUBO.color = { 0.0, 0.0, 0.0 };
		}
		
		void* hudMatrixData;
		unsigned int uiUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::UI].descriptorsBindingsIDs[0];
        vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[uiUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(UI_UBO) * offset,
					sizeof(UI_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(UI_UBO));
        vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[uiUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

	void CVulkanRenderer::updateUBO_IconsUI(uint32_t offset,
											ecs::components::transform* itemTransfromComponent,
											[[maybe_unused]] ecs::components::collider* itemColliderComponent,
											ecs::components::item* itemComponent,
											const unsigned int rowInventory,
											const unsigned int columnInventory,
											ecs::components::transform* inventoryTransformComponent,
											int itemEntity) {
		UI_UBO hudUBO{};
		float x_result_offset = 0.0f;
		float y_result_offset = 0.0f;
		if ( itemComponent->occupiedSlots.GetSize() == 0 ) {
		} else {
			const unsigned int inventorySlotEntity_0 = itemComponent->occupiedSlots[0];
			const unsigned int inventorySlotEntity_3 = itemComponent->occupiedSlots.GetHead();
			const unsigned int rowIndexFirstSlot = inventorySlotEntity_0 / rowInventory;
			const unsigned int colIndexFirstSlot = inventorySlotEntity_0 % columnInventory;
			const unsigned int rowIndexSecondSlot = inventorySlotEntity_3 / rowInventory;
			const unsigned int colIndexSecondSlot = inventorySlotEntity_3 % columnInventory;

			const float itemScale             = itemTransfromComponent->scale;
			const float fullSlotScale         = itemTransfromComponent->gltf ? itemScale * 2.0f : itemScale;
			constexpr float centreMultiplayer = 0.5f;                                                                   ///< Eather division by 2.0f using multiply on 0.5f
			x_result_offset = inventoryTransformComponent->position[0] + (colIndexFirstSlot * fullSlotScale + colIndexSecondSlot * fullSlotScale) * centreMultiplayer;
			y_result_offset = inventoryTransformComponent->position[1] + (rowIndexFirstSlot * fullSlotScale + rowIndexSecondSlot * fullSlotScale) * centreMultiplayer * aspectRate;
		}
		float itemScale = itemTransfromComponent->scale;

		if ( dragedItemEntity != itemEntity ) {
			itemTransfromComponent->position = vec3(x_result_offset, y_result_offset, 0.1f);
		} else {
//			std::cout << "item entity: " << itemEntity << std::endl;
			itemScale *= 1.1f;
//			itemColliderComponent->itemDrag = false;
			itemTransfromComponent->position[2] = 0.0f;
		}

//		std::cout << itemTransfromComponent->position << std::endl;
		
		mat4 model(1.0);
		model[0][0] = itemScale * itemComponent->itemSlotType.width;
		model[1][1] = itemScale * itemComponent->itemSlotType.height;
		model[2][2] = 0.0f;
		model[3][0] = itemTransfromComponent->position[0];
		model[3][1] = itemTransfromComponent->position[1];
		model[3][2] = itemTransfromComponent->position[2];

		hudUBO.model = model;
		
		void* hudMatrixData;
		unsigned int uiIconsUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::UI_ICONS].descriptorsBindingsIDs[0];
        vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[uiIconsUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(UI_UBO) * offset,
					sizeof(UI_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(UI_UBO));
        vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[uiIconsUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}
	
    void CVulkanRenderer::hudRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

		namespace cm = GLVM::ecs::components;
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::health>();
		
//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);
		
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPasses[SpecificPipeline::HUD_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::HUD_PIPELINE].pipeline);

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

		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			unsigned int uiEntity = linkedEntities[i];
			unsigned int uiVertexId = componentManager->GetComponent<ecs::components::mesh>(0)->handle.id;
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(uiEntity);
			cm::health* healthComponent = componentManager->GetComponent<cm::health>(uiEntity);

			unsigned int uboIndex = currentFrame * hudUboDescriptorNumber + i;
			updateHudUBO(uboIndex, transformComponent, healthComponent, true, highest_gltf_Y[uiVertexId]);
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::HUD_PIPELINE].linkedDescriptorSetIDs[0];
  			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::HUD_PIPELINE].pipelineLayout,
									0, 1, &(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + i)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = aIndices_[uiVertexId].size();

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

    void CVulkanRenderer::uiRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

		namespace cm = GLVM::ecs::components;
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::inventory>();
		
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPasses[SpecificPipeline::UI_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::UI_PIPELINE].pipeline);

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

//		core::vector<Entity> viewPositionLinkedEntities = componentManager->collectLinkedEntities<cm::beholder>();

		// cm::transform* playerTransformComponent = nullptr;

		// if ( viewPositionLinkedEntities.GetSize() > 0 )
		// 	playerTransformComponent = componentManager->GetComponent<cm::transform>(viewPositionLinkedEntities[0]);

		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
//			std::cout << "i: " << i << std::endl;
			unsigned int uiEntity = linkedEntities[i];
			cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(uiEntity);
			unsigned int uiVertexId           = inventoryComponent->slotMeshID.id;
//			std::cout << "PRINT INVENTORY SLOTS INDIXES" << std::endl;
			for ( unsigned int j = 0; j < inventoryComponent->row; ++j ) {
				for ( unsigned int m = 0; m < inventoryComponent->col; ++m ) {
//					std::cout << "slots index: " << (j / 8) + (j % 8) << " with entity: " << inventoryComponent->slots[j][m] << std::endl;
					unsigned int inventorySlotEntity = inventoryComponent->slots[j][m];
					// cm::mesh* inventorySlotMeshComponent = componentManager->GetComponent<cm::mesh>(inventorySlotEntity);
					// unsigned int uiVertexId = inventorySlotMeshComponent->handle.id;
					cm::transform* slotTransformComponent     = componentManager->GetComponent<cm::transform>(uiEntity);
//					cm::transform* inventorySlotTransformComponent = componentManager->GetComponent<cm::transform>(inventorySlotEntity);

					unsigned int uboIndex = currentFrame * uiUboDescriptorsNumber + j * inventoryComponent->col + m;
					updateUBO_UI(j, m,
								 uboIndex, inventoryComponent->slotScale, inventorySlotEntity, slotTransformComponent);
					const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::UI_PIPELINE].linkedDescriptorSetIDs[0];
					const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::UI_PIPELINE].pipelineLayout,
											0, 1, &(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

					const unsigned int linkedDescriptorSetID1 = pipelineConfigs[SpecificPipeline::UI_PIPELINE].linkedDescriptorSetIDs[1];
					const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetID1];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::UI_PIPELINE].pipelineLayout,
											1, 1, &(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + currentFrame)), 0, nullptr);
//			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(uiEntity);
					// unsigned int diffuseTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->diffuseTextureID_.id;
					// unsigned int specularTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->specularTextureID_.id;
//			cm::material* materialComponent = componentManager->GetComponent<cm::material>(uiEntity);
				
					// unsigned int uboIndex = i;
					// updateMatrixUniformBuffer(currentFrame, uboIndex, transformComponent, uiVertexId, materialComponent);
					// vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, hudPipeline.pipelineLayout,
					// 						0, 1, &hudDescriptorSets[uboIndex], 0, nullptr);

					VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

					vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

					unsigned int indicesContainerSize = aIndices_[uiVertexId].size();

					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
				}
			}
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

    void CVulkanRenderer::uiIconsRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

		namespace cm = GLVM::ecs::components;
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::mesh, cm::material, cm::transform, cm::collider, cm::item>();
		
//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);
		
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPasses[SpecificPipeline::UI_ICONS_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].pipeline);

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

		core::vector<unsigned int> inventoryEntities = componentManager->collectLinkedEntities<cm::inventory>();
		cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntities[0]);
		cm::transform* inventoryTransformComponent = componentManager->GetComponent<cm::transform>(inventoryEntities[0]);
		
		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			unsigned int itemEntity = linkedEntities[i];
			cm::item* itemComponent = componentManager->GetComponent<cm::item>(itemEntity);
			cm::actor* actorComponent = componentManager->GetComponent<cm::actor>(itemEntity);
			if ( actorComponent != nullptr )
				continue;
			
			unsigned int uiVertexId = componentManager->GetComponent<ecs::components::mesh>(itemEntity)->handle.id;
			unsigned int diffuseTexureID = componentManager->GetComponent<ecs::components::material>(itemEntity)->diffuseTextureID_.id;
			cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(itemEntity);
			cm::collider* itemColliderComponent = componentManager->GetComponent<cm::collider>(itemEntity);

			unsigned int uboIndex = currentFrame * MAX_FRAMES_IN_FLIGHT + i;
			if ( itemTransformComponent == nullptr )
				std::cout << "NULL POINTER" << std::endl;
			
			updateUBO_IconsUI(uboIndex, itemTransformComponent, itemColliderComponent, itemComponent, inventoryComponent->row, inventoryComponent->col, inventoryTransformComponent, itemEntity);
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].linkedDescriptorSetIDs[0];
  			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].pipelineLayout,
									0, 1, &(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

			const unsigned int linkedDescriptorSetID1 = pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].linkedDescriptorSetIDs[1];
  			const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetID1];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].pipelineLayout,
									1, 1, &(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + MAX_FRAMES_IN_FLIGHT * diffuseTexureID + currentFrame)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = aIndices_[uiVertexId].size();

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }
	
    void CVulkanRenderer::hudScreenRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

		namespace cm = GLVM::ecs::components;
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
		
//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);
		
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPasses[SpecificPipeline::HUD_SCREEN_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::HUD_SCREEN_PIPELINE].pipeline);

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

		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			unsigned int uiEntity = linkedEntities[i];
			cm::transform* cursorTransform = componentManager->GetComponent<ecs::components::transform>(uiEntity);
			unsigned int uiVertexId = componentManager->GetComponent<ecs::components::mesh>(uiEntity)->handle.id;

			unsigned int uboIndex = currentFrame * hudScreenUboDescriptorNumber + i;
			updateHudScreenUBO(uboIndex, cursorTransform);
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::HUD_SCREEN_PIPELINE].linkedDescriptorSetIDs[0];
  			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::HUD_SCREEN_PIPELINE].pipelineLayout,
									0, 1, &(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = aIndices_[uiVertexId].size();

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
	
    void CVulkanRenderer::fontRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

		namespace cm = GLVM::ecs::components;
		
//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);
		
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPasses[SpecificPipeline::FONT_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::FONT_PIPELINE].pipeline);

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

		core::vector<Entity> viewPositionLinkedEntities = componentManager->collectLinkedEntities<cm::beholder>();

		cm::transform* playerTransformComponent = nullptr;
		if ( viewPositionLinkedEntities.GetSize() > 0 )
			playerTransformComponent = componentManager->GetComponent<cm::transform>(viewPositionLinkedEntities[0]);

		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::font>();
		unsigned int currentActorMemoryOffset = 0;
		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			unsigned int entity = linkedEntities[i];
			cm::font* fontComponent = componentManager->GetComponent<cm::font>(entity);
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(entity);
			vec3 playerTragetDirection = transformComponent->position - playerTransformComponent->position;
			float dotProduct = Dot(playerTragetDirection, playerTransformComponent->forward);
			if ( dotProduct <= 0 )
				continue;

			for ( unsigned int j = 0; j < fontComponent->font_string.GetSize(); ++j ) {
				unsigned int ascii_code = static_cast<unsigned int>(fontComponent->font_string[j]);
				VkBuffer vertexBuffers[] = { fontVertexBufferContainer[ascii_code] };
				VkDeviceSize offsets[] = {0};
				
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffer, fontIndexBufferContainer[ascii_code], 0, VK_INDEX_TYPE_UINT32);

				unsigned int indicesContainerSize = symbol_g_indices.size();
				FONT_UBO fontUBO{};
				vec3 result;
				vec4 pos = vec4(transformComponent->position[0],
								transformComponent->position[1],
								transformComponent->position[2], 1.0f);

				vec4 clipSpacePosition =  pos * viewMatrix * projectionMatrix;
				vec3 ndcPosition = vec3(clipSpacePosition[0] / clipSpacePosition[3],
										clipSpacePosition[1] / clipSpacePosition[3],
										clipSpacePosition[2] / clipSpacePosition[3]);

				fontUBO.view = viewMatrix;
				fontUBO.proj = projectionMatrix;

				fontUBO.scale    = 0.3f;
				ndcPosition[0] += (float)j * 0.17f * fontUBO.scale;
				ndcPosition[1] -= fontComponent->lifeTime / 5.0f;
				fontUBO.position = ndcPosition;

				void* modelMatrixData;
				unsigned int fontUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::FONT_RENDER_UBO].descriptorsBindingsIDs[0];
				vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[fontUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(fontUBO) * (currentActorMemoryOffset + j),
							sizeof(fontUBO), 0, &modelMatrixData);
				memcpy(modelMatrixData, &fontUBO, sizeof(fontUBO));
				vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[fontUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

				const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::FONT_PIPELINE].linkedDescriptorSetIDs[0];
				const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::FONT_PIPELINE].pipelineLayout, 0, 1,
										&(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + currentActorMemoryOffset + j)), 0, nullptr);
				const unsigned int linkedDescriptorSetID1 = pipelineConfigs[SpecificPipeline::FONT_PIPELINE].linkedDescriptorSetIDs[1];
				const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetID1];
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::FONT_PIPELINE].pipelineLayout, 1, 1,
										&(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset)), 0, nullptr);
			
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
			}
			currentActorMemoryOffset += currentFrame * fontUboDescriptorNumber + fontComponent->font_string.GetSize();
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }
	
    void CVulkanRenderer::recordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

		namespace cm = GLVM::ecs::components;
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::material,
																						   cm::mesh,
																						   cm::actor>();
		
		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);
		
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPasses[SpecificPipeline::MAIN_RENDER_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipeline);

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

		core::vector<Entity> viewPositionLinkedEntities = componentManager->collectLinkedEntities<cm::beholder>();

		cm::transform* playerTransformComponent = nullptr;

		if ( viewPositionLinkedEntities.GetSize() > 0 )
			playerTransformComponent = componentManager->GetComponent<cm::transform>(viewPositionLinkedEntities[0]);

		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			unsigned int uiEntity = linkedEntities[i];
			unsigned int uiVertexId = componentManager->GetComponent<ecs::components::mesh>(uiEntity)->handle.id;
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(uiEntity);
			unsigned int diffuseTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->diffuseTextureID_.id;
			unsigned int specularTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->specularTextureID_.id;
			cm::material* materialComponent = componentManager->GetComponent<cm::material>(uiEntity);
				
			unsigned int uboIndex = currentFrame * matrixUboDescriptorsNumber + i;
			updateMatrixUniformBuffer(uboIndex, transformComponent, uiVertexId, materialComponent);
			const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[0];
  			const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout,
									0, 1, &(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

			updateViewPositionUniformBuffer(currentFrame, playerTransformComponent);
			const unsigned int linkedDescriptorSetID1 = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[1];
  			const DescriptorSet& currentDescriptorSet1 = descriptorSetsConfig[linkedDescriptorSetID1];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout,
									1, 1, &(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + currentFrame)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = aIndices_[uiVertexId].size();

//			updateSamplersDescriptroSets(diffuseTextureIndex, specularTextureIndex);
			const unsigned int linkedDescriptorSetID2 = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[2];
  			const DescriptorSet& currentDescriptorSet2 = descriptorSetsConfig[linkedDescriptorSetID2];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout, 2, 1,
									&(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet2.descriptorSetOffset + MAX_FRAMES_IN_FLIGHT * specularTextureIndex + currentFrame)), 0, nullptr);
			const unsigned int linkedDescriptorSetID3 = pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[3];
  			const DescriptorSet& currentDescriptorSet3 = descriptorSetsConfig[linkedDescriptorSetID3];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout, 3, 1,
									&(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet3.descriptorSetOffset + MAX_FRAMES_IN_FLIGHT * diffuseTextureIndex + currentFrame)), 0, nullptr);
			
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

    void CVulkanRenderer::createSyncObjects(std::vector<VkSemaphore>& imageAvailableSemaphores,
											std::vector<VkSemaphore>& renderFinishedSemaphores,
											std::vector<VkFence>& inFlightFences) {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(swapChainImages.size());
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }

		for (size_t i = 0; i < swapChainImages.size(); ++i) {
            if ( vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

	void CVulkanRenderer::updateDirectionalLightSpaceMatrixShadowMapUBO(ecs::components::directionalLight* directionalLightComponent,
																		uint32_t currentLight) {
		float nearPlaneFlatShadowMap = 5.5f;
		float farPlaneFlatShadowMap = 100.0f;
		mat4 directionalProjectionMatrixLight = ortho(-50.0f, 50.0f, -50.0f, 50.0f,
													  nearPlaneFlatShadowMap, farPlaneFlatShadowMap);

		vec3 positionVectorLight = directionalLightComponent->position;
		vec3 directionVectorLight = directionalLightComponent->direction;

		mat4 viewMatrixLight = LookAtMain(positionVectorLight,
										  directionVectorLight,
										  { 0.0f, -1.0f, 0.0f });

//		directionalProjectionMatrixLight[1][1] *= -1;
		dirLightSpaceMatrix[currentLight] = viewMatrixLight * directionalProjectionMatrixLight;
	}
	
    void CVulkanRenderer::updateDirectionalLightShadowMapMatrixUBO(uint32_t currentImage, [[maybe_unused]] ecs::components::transform* _transformComponent,
																   [[maybe_unused]] uint32_t currentLight, [[maybe_unused]] u32 meshID) {
		ShadowMapMatrixUBO modelMatrixUBO{};

        modelMatrixUBO.model = computeModelMatrix(_transformComponent);
		modelMatrixUBO.lightSpaceMatrix = dirLightSpaceMatrix[currentLight];

		mat4* jointMatricesData = updateAnimationFrames(_transformComponent, meshID);

		for ( unsigned int j = 0; j < MAX_JOINTS_NUMBER; ++j ) {
			modelMatrixUBO.jointMatrices[j] = jointMatricesData[j];
		}

		delete [] jointMatricesData;
		jointMatricesData = nullptr;
		
        void* modelMatrixData = nullptr;
		unsigned int shadowMapDirectionalLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_DIRECTIONAL_LIGHT].descriptorsBindingsIDs[0];		
        vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[shadowMapDirectionalLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, currentImage * sizeof(modelMatrixUBO),
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[shadowMapDirectionalLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
    }

	void CVulkanRenderer::updateSpotLightSpaceMatrixShadowMapUBO(ecs::components::spotLight* spotLightComponent,
																		uint32_t currentLight) {
		float nearPlaneFlatShadowMap = 5.5f;
		float farPlaneFlatShadowMap = 100.0f;
		mat4 spotProjectionMatrixLight = Perspective(Radians(90.0f), (float)SHADOW_MAP_SIZE / (float)SHADOW_MAP_SIZE,
														 nearPlaneFlatShadowMap, farPlaneFlatShadowMap);
		
		vec3 positionVectorLight  = spotLightComponent->position;
		vec3 directionVectorLight = spotLightComponent->direction;
		mat4 viewMatrixLight = LookAtMain(positionVectorLight,
										  directionVectorLight,
										  { 0.0f, -1.0f, 0.0f });

//		spotProjectionMatrixLight[1][1] *= -1;
		spotLightSpaceMatrix[currentLight] = viewMatrixLight * spotProjectionMatrixLight;
	}
	
    void CVulkanRenderer::updateSpotLightShadowMapMatrixUBO(uint32_t currentImage, [[maybe_unused]] ecs::components::transform* _transformComponent, [[maybe_unused]] uint32_t currentLight, [[maybe_unused]] u32 meshID) {
		ShadowMapMatrixUBO modelMatrixUBO{};
		
        modelMatrixUBO.model = computeModelMatrix(_transformComponent);
		modelMatrixUBO.lightSpaceMatrix = spotLightSpaceMatrix[currentLight];

		mat4* jointMatricesData = updateAnimationFrames(_transformComponent, meshID);
		
		for ( unsigned int j = 0; j < MAX_JOINTS_NUMBER; ++j ) {
			modelMatrixUBO.jointMatrices[j] = jointMatricesData[j];
		}

		delete [] jointMatricesData;
		jointMatricesData = nullptr;
		
        void* modelMatrixData;
		unsigned int shadowMapSpotLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_SPOT_LIGHT].descriptorsBindingsIDs[0];		
        vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[shadowMapSpotLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, currentImage * sizeof(modelMatrixUBO),
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[shadowMapSpotLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
    }

    void CVulkanRenderer::updatePointLightShadowMapMatrixUBO([[maybe_unused]] uint32_t currentImage, ecs::components::transform* _transformComponent, ecs::components::pointLight* pointLightComponent, uint32_t layer, unsigned int meshID) {
		PointLightShadowMapMatrixUBO modelMatrixUBO{};

		vec3 positionVectorLight  = pointLightComponent->position;
		vec3 directionalVectorLight = vec3(0.0f, 0.0f, 0.0f);
		vec3 upVector = { 0.0, 0.0, 0.0 };

		switch(layer) {
		case 0:
			/// Positive X
			directionalVectorLight = positionVectorLight + vec3( 1.0f,  0.0f, 0.0f);
			upVector = vec3(0.0f, -1.0f,  0.0f);
			break;
		case 1:
			/// Negative X
			directionalVectorLight = positionVectorLight + vec3( -1.0f,  0.0f,  0.0f);
			upVector = vec3(0.0f, -1.0f,  0.0f);
			break;
		case 2:
			/// Positive Y
			directionalVectorLight = positionVectorLight + vec3( 0.0f,  1.0f,  0.0f);
			upVector = vec3(0.0f, 0.0f,  1.0f);
			break;
		case 3:
			/// Negative Y
			directionalVectorLight = positionVectorLight + vec3( 0.0f,  -1.0f,  0.0f);
			upVector = vec3(0.0f, 0.0f,  -1.0f);
			break;
		case 4:
			/// Positive Z
			directionalVectorLight = positionVectorLight + vec3( 0.0f,  0.0f,  1.0f);
			upVector = vec3(0.0f, -1.0f,  0.0f);
			break;
			/// Negative Z
		case 5:
			directionalVectorLight = positionVectorLight + vec3( 0.0f,  0.0f,  -1.0f);
			upVector = vec3(0.0f, -1.0f,  0.0f);
			break;
		default:
			break;
		}
		
		mat4 projectionMatrixCubeShadowMap = Perspective(Radians(90.0f), (float)SHADOW_MAP_SIZE / (float)SHADOW_MAP_SIZE, 0.3f, 100.0f);

		mat4 viewMatrixLight = LookAtMain(positionVectorLight,
										  directionalVectorLight,
										  upVector);

        modelMatrixUBO.model = computeModelMatrix(_transformComponent);
		
//		projectionMatrixCubeShadowMap[1][1] *= -1;
		
		modelMatrixUBO.lightSpaceMatrix = viewMatrixLight * projectionMatrixCubeShadowMap;
		modelMatrixUBO.farPlane = 100.0f;
		modelMatrixUBO.lightPosition = positionVectorLight;

		mat4* jointMatricesData = updateAnimationFrames(_transformComponent, meshID);
		
		for ( unsigned int j = 0; j < MAX_JOINTS_NUMBER; ++j ) {
			modelMatrixUBO.jointMatrices[j] = jointMatricesData[j];
		}
		
		delete [] jointMatricesData;
		jointMatricesData = nullptr;
		
        void* modelMatrixData;
		unsigned int shadowMapPointLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_POINT_LIGHT].descriptorsBindingsIDs[0];		
        vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[shadowMapPointLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, currentImage * sizeof(modelMatrixUBO),
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[shadowMapPointLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
    }

    void CVulkanRenderer::updateMatrixUniformBuffer(uint32_t offset, ecs::components::transform* _transformComponent,
													unsigned int meshID, ecs::components::material* materialComponent) {
        ModelMatrixUBO modelMatrixUBO{};
		
        modelMatrixUBO.model = computeModelMatrix(_transformComponent);
		
        modelMatrixUBO.view = viewMatrix;
        modelMatrixUBO.proj = projectionMatrix;

		/// Start of animation logic
		mat4* jointMatricesData = updateAnimationFrames(_transformComponent, meshID);

		for ( unsigned int j = 0; j < MAX_JOINTS_NUMBER; ++j ) {
			modelMatrixUBO.jointMatrices[j] = jointMatricesData[j];
		}
		
		delete [] jointMatricesData;
		jointMatricesData = nullptr;
		/// End of animation logic

		modelMatrixUBO.ambient = materialComponent->ambient;
		modelMatrixUBO.shininess = materialComponent->shininess;

		for ( uint32_t i = 0; i < directionalLightNumber; ++i )
			modelMatrixUBO.dirSpaceMatrix[i] = dirLightSpaceMatrix[i];

		for ( uint32_t i = 0; i < spotLightNumber; ++i )
			modelMatrixUBO.spotSpaceMatrix[i] = spotLightSpaceMatrix[i];
		
		modelMatrixUBO.directionalLightsNumber = directionalLightNumber;
		modelMatrixUBO.spotLightsNumber        = spotLightNumber;
		
        void* modelMatrixData;
        vkMapMemory(device, GPUDescriptors[DescriptorSetDataLink::MAIN_RENDER_MATRIX_UBO].GPUBuffer->deviceMemory, sizeof(modelMatrixUBO) * offset,
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, GPUDescriptors[DescriptorSetDataLink::MAIN_RENDER_MATRIX_UBO].GPUBuffer->deviceMemory);
    }

	void CVulkanRenderer::updateViewPositionUniformBuffer(uint32_t currentImage, ecs::components::transform* transformComponent) {
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
		LightData lightDataUBO{};
		core::vector<Entity> pointLightEntities = componentManager->collectLinkedEntities<GLVM::ecs::components::controller>();
		if ( pointLightEntities.GetSize() > 0 )

			lightDataUBO.viewPosition = transformComponent->position;

		DirectionalLight directionalLight{};

		core::vector<Entity> linkedEntitiesDirLight      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::directionalLight,
																						   cm::mesh>();

		directionalLightNumber = linkedEntitiesDirLight.GetSize();
		assert(directionalLightNumber <= 4 && "Directional lights number greater then 4");

		for ( unsigned int i = 0; i < directionalLightNumber; ++i ) {
			cm::directionalLight* directionalLightComponent = componentManager->GetComponent<cm::directionalLight>(linkedEntitiesDirLight[i]);
			
			directionalLight.position  = vec4(directionalLightComponent->position[0],
											  directionalLightComponent->position[1],
											  directionalLightComponent->position[2], 0.0f);
			directionalLight.direction = vec4(directionalLightComponent->direction[0],
											  directionalLightComponent->direction[1],
											  directionalLightComponent->direction[2], 0.0f);
			directionalLight.ambient   = vec4(directionalLightComponent->ambient[0],
											  directionalLightComponent->ambient[1],
											  directionalLightComponent->ambient[2], 0.0f);
			directionalLight.diffuse   = vec4(directionalLightComponent->diffuse[0],
											  directionalLightComponent->diffuse[1],
											  directionalLightComponent->diffuse[2], 0.0f);
			directionalLight.specular  = vec4(directionalLightComponent->specular[0],
											  directionalLightComponent->specular[1],
											  directionalLightComponent->specular[2], 0.0f);

			lightDataUBO.directionalLights[i] = directionalLight;			
		}

		lightDataUBO.directionalLightsArraySize = directionalLightNumber;

		core::vector<Entity> linkedEntitiesPointLight      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::pointLight,
																						   cm::mesh>();

		pointLightNumber = linkedEntitiesPointLight.GetSize();
		assert(pointLightNumber <= POINT_LIGHTS_NUMBER && "Point lights number greater than 32");
		for ( unsigned int i = 0; i < pointLightNumber; ++i ) {
			cm::pointLight* pointLightComponent = componentManager->GetComponent<cm::pointLight>(linkedEntitiesPointLight[i]);
			PointLight pointLightUBO{};

 			pointLightUBO.position  = vec3(pointLightComponent->position[0],
										   pointLightComponent->position[1],
										   pointLightComponent->position[2]);
			pointLightUBO.ambient   = vec3(pointLightComponent->ambient[0],
										   pointLightComponent->ambient[1],
										   pointLightComponent->ambient[2]);
			pointLightUBO.diffuse   = vec3(pointLightComponent->diffuse[0],
										   pointLightComponent->diffuse[1],
										   pointLightComponent->diffuse[2]);
			pointLightUBO.specular  = pointLightComponent->specular;

			pointLightUBO.constant  = pointLightComponent->constant;

			pointLightUBO.linear    = pointLightComponent->linear;

			pointLightUBO.quadratic = pointLightComponent->quadratic;

			lightDataUBO.pointLights[i] = pointLightUBO;
		}
		
		lightDataUBO.pointLightsArraySize = pointLightNumber;
		lightDataUBO.farPlane = 100.0f;

		SpotLight spotLight{};
		core::vector<Entity> linkedEntitiesSpotLight      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::spotLight,
																						   cm::mesh>();

		spotLightNumber = linkedEntitiesSpotLight.GetSize();
		assert(spotLightNumber <= 8 && "Spot light number greater then 8");
		for ( unsigned int i = 0; i < spotLightNumber; ++i ) {
			cm::spotLight* spotLightComponent = componentManager->GetComponent<cm::spotLight>(linkedEntitiesSpotLight[i]);
			
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

			lightDataUBO.spotLights[i] = spotLight;
		}

		lightDataUBO.spotLightArraySize = spotLightNumber;

		std::random_device rd;
		std::mt19937 mersenne(rd());
		std::uniform_int_distribution<int> distributionTileIndex(0, INDIRECT_TEXTURE_HEIGHT * INDIRECT_TEXTURE_WIDTH);

		if( print == true ) {
		for( int i = 0; i < INDIRECT_TEXTURE_HEIGHT * INDIRECT_TEXTURE_WIDTH / 4 + 1; ++i )
			for( int j = 0; j < 4; ++j ) {
				int randomTileIndex = distributionTileIndex(mersenne);
//				randomTileIndex = 20;
				indirectTexture[i][j] = randomTileIndex;
				// if( print )
				// 	std::cout << "element: " << i * 4 + j << " value: " << indirectTexture[i][j] << std::endl;
				
				// std::cout << "index: " << i * INDIRECT_TEXTURE_WIDTH * 4 + j * 4 + 3 << std::endl;
				// lightDataUBO.indirectTexture[i * INDIRECT_TEXTURE_WIDTH * 4 + j * 4 + 3] = randomTileIndex;
//				std::cout << "element: " << i * INDIRECT_TEXTURE_WIDTH + j << " equal: " << lightDataUBO.indirectTexture[i * INDIRECT_TEXTURE_WIDTH + j] << std::endl;
			}
		}
		print = false;
		// if( print == true )
		// 	print = false;

		lightDataUBO.tilesetTilesCount = vec2(TILESET_ROW, TILESET_COLUMN);
		lightDataUBO.tilesRaw = 8;
		lightDataUBO.tilesColumn = 8;
		for( int i = 0; i < INDIRECT_TEXTURE_HEIGHT * INDIRECT_TEXTURE_WIDTH / 4 + 1; ++i ) {
			lightDataUBO.indirectTexture[i] = indirectTexture[i];
		}
		
        void* data;
		unsigned int lightDataUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[0];		
        vkMapMemory(device, GPUDescriptors[descriptorBindingsConfig[lightDataUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(lightDataUBO) * currentImage,
					sizeof(lightDataUBO), 0, &data);
        memcpy(data, &lightDataUBO, sizeof(lightDataUBO));
        vkUnmapMemory(device, GPUDescriptors[descriptorBindingsConfig[lightDataUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

    void CVulkanRenderer::hudDrawFrame() {
		namespace cm = GLVM::ecs::components;
		// mutex0.lock();
		// mutex0.unlock();
		// mutex1.lock();
		// mutex1.unlock();
		// mutex2.lock();
		// mutex2.unlock();
        vkWaitForFences(device, 1, &hudInFlightFences[hudCurrentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, hudImageAvailableSemaphores[hudCurrentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &hudInFlightFences[hudCurrentFrame]);
        vkResetCommandBuffer(hudCommandBuffers[hudCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        hudRecordCommandBuffer(hudCommandBuffers[hudCurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {hudImageAvailableSemaphores[hudCurrentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &hudCommandBuffers[hudCurrentFrame];

        VkSemaphore signalSemaphores[] = {hudRenderFinishedSemaphores[hudCurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, hudInFlightFences[hudCurrentFrame]) != VK_SUCCESS) {
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

        hudCurrentFrame = (hudCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void CVulkanRenderer::fontDrawFrame() {
		namespace cm = GLVM::ecs::components;
		// mutex0.lock();
		// mutex0.unlock();
		// mutex1.lock();
		// mutex1.unlock();
		// mutex2.lock();
		// mutex2.unlock();
        vkWaitForFences(device, 1, &fontInFlightFences[fontCurrentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, fontImageAvailableSemaphores[fontCurrentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &fontInFlightFences[fontCurrentFrame]);
        vkResetCommandBuffer(fontCommandBuffers[fontCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        fontRecordCommandBuffer(fontCommandBuffers[fontCurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {fontImageAvailableSemaphores[fontCurrentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &fontCommandBuffers[fontCurrentFrame];

        VkSemaphore signalSemaphores[] = {fontRenderFinishedSemaphores[fontCurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fontInFlightFences[fontCurrentFrame]) != VK_SUCCESS) {
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

        fontCurrentFrame = (fontCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
	
    void CVulkanRenderer::mainRenderDrawFrame() {
		namespace cm = GLVM::ecs::components;
		// mutex0.lock();
		// mutex0.unlock();
		// mutex1.lock();
		// mutex1.unlock();
		// mutex2.lock();
		// mutex2.unlock();
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
		/* vkAcquireNextImageKHR give index of image that WILL BE SOON available for rendering and signal imageAvailablesemaphore when its so.
		   GraphicsQueue waint for this semaphore bacause we pass it in submitInfo.
		 */
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &inFlightFences[currentFrame]);
        vkResetCommandBuffer(mainRenderCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		hudRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		fontRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		if ( isInventoryOpened ) {
			uiRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
			uiIconsRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		}
		
		hudScreenRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		/// GraphicsQueue wait for swapchain image when its become available.
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mainRenderCommandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[imageIndex]};
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
//		currentFrame = 0;
    }

    void CVulkanRenderer::directionalLightShadowMapDrawFrame() {
		namespace cm = GLVM::ecs::components;
//		shadowMapPassesMutex.lock();
        vkWaitForFences(device, 1, &directionalLightShadowMapInFlightFences[directionalLightCurrentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        // VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, directionalLightShadowMapImageAvailableSemaphores[directionalLightCurrentFrame], VK_NULL_HANDLE, &imageIndex);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //     recreateSwapChain();
        //     return;
        // } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        //     throw std::runtime_error("failed to acquire swap chain image!");
        // }

        vkResetFences(device, 1, &directionalLightShadowMapInFlightFences[directionalLightCurrentFrame]);
        vkResetCommandBuffer(directionalLightCommandBuffers[directionalLightCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        directionalLightRecordCoomandBuffer(directionalLightCommandBuffers[directionalLightCurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // VkSemaphore waitSemaphores[] = {directionalLightShadowMapImageAvailableSemaphores[directionalLightCurrentFrame]};
        // VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        // submitInfo.waitSemaphoreCount = 1;
        // submitInfo.pWaitSemaphores = waitSemaphores;
        // submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &directionalLightCommandBuffers[directionalLightCurrentFrame];

        // VkSemaphore signalSemaphores[] = {directionalLightShadowMapRenderFinishedSemaphores[directionalLightCurrentFrame]};
        // submitInfo.signalSemaphoreCount = 1;
        // submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, directionalLightShadowMapInFlightFences[directionalLightCurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // VkPresentInfoKHR presentInfo{};
        // presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        // presentInfo.waitSemaphoreCount = 1;
        // presentInfo.pWaitSemaphores = signalSemaphores;

        // VkSwapchainKHR swapChains[] = {swapChain};
        // presentInfo.swapchainCount = 1;
        // presentInfo.pSwapchains = swapChains;

        // presentInfo.pImageIndices = &imageIndex;

        // result = vkQueuePresentKHR(presentQueue, &presentInfo);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        //     framebufferResized = false;
        //     recreateSwapChain();
        // } else if (result != VK_SUCCESS) {
        //     throw std::runtime_error("failed to present swap chain image!");
        // }

        directionalLightCurrentFrame = (directionalLightCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		// shadowMapPassesMutex.unlock();
		// mutex0.unlock();
    }

	void CVulkanRenderer::spotLightShadowMapDrawFrame() {
		namespace cm = GLVM::ecs::components;
//		shadowMapPassesMutex.lock();
        vkWaitForFences(device, 1, &spotLightShadowMapInFlightFences[spotLightCurrentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        // VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, spotLightShadowMapImageAvailableSemaphores[spotLightCurrentFrame], VK_NULL_HANDLE, &imageIndex);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //     recreateSwapChain();
        //     return;
        // } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        //     throw std::runtime_error("failed to acquire swap chain image!");
        // }

        vkResetFences(device, 1, &spotLightShadowMapInFlightFences[spotLightCurrentFrame]);
        vkResetCommandBuffer(spotLightCommandBuffers[spotLightCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        spotLightRecordCommandBuffer(spotLightCommandBuffers[spotLightCurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // VkSemaphore waitSemaphores[] = {spotLightShadowMapImageAvailableSemaphores[spotLightCurrentFrame]};
        // VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        // submitInfo.waitSemaphoreCount = 1;
        // submitInfo.pWaitSemaphores = waitSemaphores;
        // submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &spotLightCommandBuffers[spotLightCurrentFrame];

        // VkSemaphore signalSemaphores[] = {spotLightShadowMapRenderFinishedSemaphores[spotLightCurrentFrame]};
        // submitInfo.signalSemaphoreCount = 1;
        // submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, spotLightShadowMapInFlightFences[spotLightCurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // VkPresentInfoKHR presentInfo{};
        // presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        // presentInfo.waitSemaphoreCount = 1;
        // presentInfo.pWaitSemaphores = signalSemaphores;

        // VkSwapchainKHR swapChains[] = {swapChain};
        // presentInfo.swapchainCount = 1;
        // presentInfo.pSwapchains = swapChains;

        // presentInfo.pImageIndices = &imageIndex;

        // result = vkQueuePresentKHR(presentQueue, &presentInfo);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        //     framebufferResized = false;
        //     recreateSwapChain();
        // } else if (result != VK_SUCCESS) {
        //     throw std::runtime_error("failed to present swap chain image!");
        // }

        spotLightCurrentFrame = (spotLightCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		// shadowMapPassesMutex.unlock();
		// mutex1.unlock();
    }

	    void CVulkanRenderer::pointLightShadowMapDrawFrame() {
		namespace cm = GLVM::ecs::components;
//		shadowMapPassesMutex.lock();
        vkWaitForFences(device, 1, &pointLightShadowMapInFlightFences[pointLightCurrentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        // VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, pointLightShadowMapImageAvailableSemaphores[pointLightCurrentFrame], VK_NULL_HANDLE, &imageIndex);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //     recreateSwapChain();
        //     return;
        // } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        //     throw std::runtime_error("failed to acquire swap chain image!");
        // }

        vkResetFences(device, 1, &pointLightShadowMapInFlightFences[pointLightCurrentFrame]);
        vkResetCommandBuffer(pointLightCommandBuffers[pointLightCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        pointLightRecordCommandBuffer(pointLightCommandBuffers[pointLightCurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // VkSemaphore waitSemaphores[] = {pointLightShadowMapImageAvailableSemaphores[pointLightCurrentFrame]};
        // VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        // submitInfo.waitSemaphoreCount = 1;
        // submitInfo.pWaitSemaphores = waitSemaphores;
        // submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &pointLightCommandBuffers[pointLightCurrentFrame];

        // VkSemaphore signalSemaphores[] = {pointLightShadowMapRenderFinishedSemaphores[pointLightCurrentFrame]};
        // submitInfo.signalSemaphoreCount = 1;
        // submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, pointLightShadowMapInFlightFences[pointLightCurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // VkPresentInfoKHR presentInfo{};
        // presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        // presentInfo.waitSemaphoreCount = 1;
        // presentInfo.pWaitSemaphores = signalSemaphores;

        // VkSwapchainKHR swapChains[] = {swapChain};
        // presentInfo.swapchainCount = 1;
        // presentInfo.pSwapchains = swapChains;

        // presentInfo.pImageIndices = &imageIndex;

        // result = vkQueuePresentKHR(presentQueue, &presentInfo);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        //     framebufferResized = false;
        //     recreateSwapChain();
        // } else if (result != VK_SUCCESS) {
        //     throw std::runtime_error("failed to present swap chain image!");
        // }

        pointLightCurrentFrame = (pointLightCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		// shadowMapPassesMutex.unlock();
		// mutex2.unlock();
    }
	
	void CVulkanRenderer::directionalLightRecordCoomandBuffer(VkCommandBuffer& commandBuffer, [[maybe_unused]] uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

		
		namespace cm = GLVM::ecs::components;
		core::vector<Entity> directionalLightEntities      = componentManager->collectLinkedEntities<cm::transform,
																									 cm::directionalLight,
																									 cm::mesh,
																									 cm::actor>();

		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::material,
																						   cm::mesh,
																						   cm::actor>();

		for ( uint32_t directionalLightCounter = 0; directionalLightCounter < directionalLightEntities.GetSize(); ++ directionalLightCounter ) {
			VkClearValue shadowMapClearValues[1];
			shadowMapClearValues[0].depthStencil.depth = 1.0f;
			shadowMapClearValues[0].depthStencil.stencil = 0;

			VkRenderPassBeginInfo shadowMapRenderPassInfo{};
			shadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			shadowMapRenderPassInfo.pNext = NULL;
			shadowMapRenderPassInfo.renderPass = renderPasses[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE];
			shadowMapRenderPassInfo.framebuffer = directionalLightShadowMapFrameBuffers[directionalLightCounter];
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

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].pipeline);

			unsigned int directionalLightEntity = directionalLightEntities[directionalLightCounter];
			cm::directionalLight* directionalLightComponent = componentManager->GetComponent<cm::directionalLight>(directionalLightEntity);
			updateDirectionalLightSpaceMatrixShadowMapUBO(directionalLightComponent, directionalLightCounter);

			uint32_t actorsNumber = linkedEntities.GetSize();
			for ( unsigned int actorCounter = 0; actorCounter < actorsNumber; ++actorCounter ) {
				unsigned int meshOwnerEntity = linkedEntities[actorCounter];
				unsigned int meshId = componentManager->GetComponent<ecs::components::mesh>(meshOwnerEntity)->handle.id;
				cm::transform* meshOwnerTransformComponent = componentManager->GetComponent<cm::transform>(meshOwnerEntity);

				unsigned int uboDirectionalLightIndex = directionalLightNumber * actorsNumber * directionalLightCurrentFrame +
					actorsNumber * directionalLightCounter + actorCounter;

				updateDirectionalLightShadowMapMatrixUBO(uboDirectionalLightIndex, meshOwnerTransformComponent, directionalLightCounter, meshId);
				const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].linkedDescriptorSetIDs[0];
				const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].pipelineLayout, 0, 1,
										&(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboDirectionalLightIndex)), 0, nullptr);
				
				VkBuffer vertexBuffers[] = {vertexBufferContainer[meshId]};
				VkDeviceSize offsets[] = {0};
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

				vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[meshId], 0, VK_INDEX_TYPE_UINT32);

				unsigned int indicesContainerSize = aIndices_[meshId].size();
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
			}
		
			vkCmdEndRenderPass(commandBuffer);
		}

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
	}

	void CVulkanRenderer::spotLightRecordCommandBuffer(VkCommandBuffer& commandBuffer, [[maybe_unused]] uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

		namespace cm = GLVM::ecs::components;
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::material,
																						   cm::mesh,
																						   cm::actor>();

		
		core::vector<Entity> spotLightEntities      = componentManager->collectLinkedEntities<cm::transform,
																							  cm::spotLight,
																							  cm::mesh,
																							  cm::actor>();

		for ( uint32_t spotLightCounter = 0; spotLightCounter < spotLightEntities.GetSize(); ++ spotLightCounter ) {
			VkClearValue spotLightShadowMapClearValues[1];
			spotLightShadowMapClearValues[0].depthStencil.depth = 1.0f;
			spotLightShadowMapClearValues[0].depthStencil.stencil = 0;

			VkRenderPassBeginInfo spotLightShadowMapRenderPassInfo{};
			spotLightShadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			spotLightShadowMapRenderPassInfo.pNext = NULL;
			spotLightShadowMapRenderPassInfo.renderPass = renderPasses[SpecificPipeline::SPOT_LIGHT_PIPELINE];
			spotLightShadowMapRenderPassInfo.framebuffer = spotLightShadowMapFrameBuffers[spotLightCounter];
			spotLightShadowMapRenderPassInfo.renderArea.offset.x = 0;
			spotLightShadowMapRenderPassInfo.renderArea.offset.y = 0;
			spotLightShadowMapRenderPassInfo.renderArea.extent.width = swapChainExtent.width;
			spotLightShadowMapRenderPassInfo.renderArea.extent.height = swapChainExtent.height;
			spotLightShadowMapRenderPassInfo.clearValueCount = 1;
			spotLightShadowMapRenderPassInfo.pClearValues = spotLightShadowMapClearValues;

			vkCmdBeginRenderPass(commandBuffer, &spotLightShadowMapRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport spotLightShadowMapViewPort;
			spotLightShadowMapViewPort.height = swapChainExtent.height;
			spotLightShadowMapViewPort.width = swapChainExtent.width;
			spotLightShadowMapViewPort.minDepth = 0.0f;
			spotLightShadowMapViewPort.maxDepth = 1.0f;
			spotLightShadowMapViewPort.x = 0;
			spotLightShadowMapViewPort.y = 0;
			vkCmdSetViewport(commandBuffer, 0, 1, &spotLightShadowMapViewPort);

			VkRect2D spotLightShadowMapScissor;
			spotLightShadowMapScissor.extent.width = swapChainExtent.width;
			spotLightShadowMapScissor.extent.height = swapChainExtent.height;
			spotLightShadowMapScissor.offset.x = 0;
			spotLightShadowMapScissor.offset.y = 0;
			vkCmdSetScissor(commandBuffer, 0, 1, &spotLightShadowMapScissor);

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].pipeline);

			unsigned int spotLightEntity = spotLightEntities[spotLightCounter];
			cm::spotLight* spotLightComponent = componentManager->GetComponent<cm::spotLight>(spotLightEntity);
			updateSpotLightSpaceMatrixShadowMapUBO(spotLightComponent, spotLightCounter);

			uint32_t actorsNumber = linkedEntities.GetSize();
			for ( unsigned int actorsCounter = 0; actorsCounter < actorsNumber; ++actorsCounter ) {
				unsigned int meshOwnerEntity = linkedEntities[actorsCounter];
				unsigned int meshID = componentManager->GetComponent<ecs::components::mesh>(meshOwnerEntity)->handle.id;
				cm::transform* meshOwnerTransformComponent = componentManager->GetComponent<cm::transform>(meshOwnerEntity);
				unsigned int uboSpotLightIndex = spotLightNumber * actorsNumber * spotLightCurrentFrame +
					actorsNumber * spotLightCounter + actorsCounter;

				updateSpotLightShadowMapMatrixUBO(uboSpotLightIndex, meshOwnerTransformComponent, spotLightCounter, meshID);
				const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].linkedDescriptorSetIDs[0];
				const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].pipelineLayout, 0, 1,
										&(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboSpotLightIndex)), 0, nullptr);
				VkBuffer vertexBuffers[] = {vertexBufferContainer[meshID]};
				VkDeviceSize offsets[] = {0};
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

				vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[meshID], 0, VK_INDEX_TYPE_UINT32);

				unsigned int indicesContainerSize = aIndices_[meshID].size();
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
			}
		
			vkCmdEndRenderPass(commandBuffer);
		}

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
	}

	void CVulkanRenderer::pointLightRecordCommandBuffer(VkCommandBuffer& commandBuffer, [[maybe_unused]] uint32_t imageIndex) {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
		ecs::EntityManager*    entityManager     = ecs::EntityManager::GetInstance();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

		namespace cm = GLVM::ecs::components;
		if ( entityManager->isEntitiesCollectionChanged && componentManager->isComponentsCollectionChanged ) {
			core::vector<Entity> linkedEntitiesTemp      = componentManager->collectLinkedEntities<cm::transform,
																							   cm::material,
																							   cm::mesh,
																							   cm::actor>();
			
			core::vector<Entity> pointLightEntities = componentManager->collectLinkedEntities<cm::transform,
																							  cm::pointLight,
																							  cm::mesh,
																							  cm::actor>();

			core::vector<unsigned int> linkedEntities;
			for ( unsigned int i = 0; i < linkedEntitiesTemp.GetSize(); ++i ) {
				unsigned int entity = linkedEntitiesTemp[i];
				for ( unsigned int j = 0; j < pointLightEntities.GetSize(); ++j ) {
					if ( entity == pointLightEntities[j] ) {
						break;
					} else if ( entity != pointLightEntities[j] && j == pointLightEntities.GetSize() - 1 ) {
						linkedEntities.Push(entity);
					}
				}
			}
//			std::cout << "number of actors: " << linkedEntities.GetSize() << std::endl;
			entitiesCollectionLinked__Trn_Mat_Mes_Act.clear();
			for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i )
				entitiesCollectionLinked__Trn_Mat_Mes_Act.Push(linkedEntities[i]);
				
			entitiesCollectionLinked__Trn_PoL_Mes_Act.clear();
			for ( unsigned int i = 0; i < pointLightEntities.GetSize(); ++i )
				entitiesCollectionLinked__Trn_PoL_Mes_Act.Push(pointLightEntities[i]);

			entityManager->isEntitiesCollectionChanged = false;
			componentManager->isComponentsCollectionChanged = false;
		}

		VkDebugUtilsLabelEXT label;
		label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		label.color[0] = 0.1;
		label.color[0] = 0.7;
		label.color[0] = 0.2;
		label.color[0] = 1.0;
		label.pLabelName = "pointLightShadowMap";
		label.pNext = NULL;
		
		CreateBeginDebugUtilsLabelEXT(instance, commandBuffer, &label);
		for ( uint32_t pointLightCounter = 0; pointLightCounter < entitiesCollectionLinked__Trn_PoL_Mes_Act.GetSize(); ++pointLightCounter ) {
			uint32_t maxCubeMapLayers = 6;
			for ( uint32_t cubeMapLayerCounter = 0; cubeMapLayerCounter < maxCubeMapLayers; ++cubeMapLayerCounter ) {                      ///< 6 is a number of cube map layers.
				VkClearValue pointLightShadowMapClearValues[2];
				pointLightShadowMapClearValues[0].depthStencil.depth = 1.0f;
				pointLightShadowMapClearValues[0].depthStencil.stencil = 0;
				pointLightShadowMapClearValues[1].color = {{0.5f, 0.5f, 0.5f, 1.0f}};

				VkRenderPassBeginInfo pointLightShadowMapRenderPassInfo{};
				pointLightShadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				pointLightShadowMapRenderPassInfo.pNext = NULL;
				pointLightShadowMapRenderPassInfo.renderPass = renderPasses[SpecificPipeline::POINT_LIGHT_PIPELINE];
				pointLightShadowMapRenderPassInfo.framebuffer = pointLightShadowMapFrameBuffers[pointLightCounter][cubeMapLayerCounter];
				pointLightShadowMapRenderPassInfo.renderArea.offset.x = 0;
				pointLightShadowMapRenderPassInfo.renderArea.offset.y = 0;
				pointLightShadowMapRenderPassInfo.renderArea.extent.width = SHADOW_MAP_SIZE;
				pointLightShadowMapRenderPassInfo.renderArea.extent.height = SHADOW_MAP_SIZE;
				pointLightShadowMapRenderPassInfo.clearValueCount = 2;
				pointLightShadowMapRenderPassInfo.pClearValues = pointLightShadowMapClearValues;

				vkCmdBeginRenderPass(commandBuffer, &pointLightShadowMapRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

				VkViewport pointLightShadowMapViewPort;
				pointLightShadowMapViewPort.height = SHADOW_MAP_SIZE;
				pointLightShadowMapViewPort.width = SHADOW_MAP_SIZE;
				pointLightShadowMapViewPort.minDepth = 0.0f;
				pointLightShadowMapViewPort.maxDepth = 1.0f;
				pointLightShadowMapViewPort.x = 0;
				pointLightShadowMapViewPort.y = 0;
				vkCmdSetViewport(commandBuffer, 0, 1, &pointLightShadowMapViewPort);

				VkRect2D pointLightShadowMapScissor;
				pointLightShadowMapScissor.extent.width = SHADOW_MAP_SIZE;
				pointLightShadowMapScissor.extent.height = SHADOW_MAP_SIZE;
				pointLightShadowMapScissor.offset.x = 0;
				pointLightShadowMapScissor.offset.y = 0;
				vkCmdSetScissor(commandBuffer, 0, 1, &pointLightShadowMapScissor);

				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].pipeline);

				unsigned int pointLightEntity = entitiesCollectionLinked__Trn_PoL_Mes_Act[pointLightCounter];
				
				cm::pointLight* pointLightComponent = componentManager->GetComponent<cm::pointLight>(pointLightEntity);
				
				uint32_t actorsNumber = entitiesCollectionLinked__Trn_Mat_Mes_Act.GetSize();
				for ( unsigned int actorCounter = 0; actorCounter < actorsNumber; ++actorCounter ) {
					unsigned int meshOwnerEntity = entitiesCollectionLinked__Trn_Mat_Mes_Act[actorCounter];
					unsigned int meshID = componentManager->GetComponent<ecs::components::mesh>(meshOwnerEntity)->handle.id;
					cm::transform* meshOwnerTransformComponent = componentManager->GetComponent<cm::transform>(meshOwnerEntity);
						
					unsigned int uboIndex = pointLightNumber *
						actorsNumber * maxCubeMapLayers * pointLightCurrentFrame +                           ///< Choose frame (first 168 or second 168)
						actorsNumber * maxCubeMapLayers * pointLightCounter +                      ///< Choose point light (i)
						maxCubeMapLayers * actorCounter + cubeMapLayerCounter;                     ///< Choose actor (m) and layer (j)

					updatePointLightShadowMapMatrixUBO(uboIndex, meshOwnerTransformComponent, pointLightComponent, cubeMapLayerCounter, meshID);
					const unsigned int linkedDescriptorSetID = pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].linkedDescriptorSetIDs[0];
					const DescriptorSet& currentDescriptorSet = descriptorSetsConfig[linkedDescriptorSetID];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].pipelineLayout, 0, 1,
											&(*(descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

					VkBuffer vertexBuffers[] = {vertexBufferContainer[meshID]};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
					
					vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[meshID], 0, VK_INDEX_TYPE_UINT32);

					unsigned int indicesContainerSize = aIndices_[meshID].size();
					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
				}
		
				vkCmdEndRenderPass(commandBuffer);
			}
		}

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
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
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR || availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
//			if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
//				std::cout << "present mode found!" << std::endl;
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D CVulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {

			VkExtent2D actualExtent{
				.width  = Window->width,
				.height = Window->height
			};
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

#ifdef VK_USE_PLATFORM_XCB_KHR
		std::vector<const char*> pRequiredExtentions = {"VK_KHR_xcb_surface",
            "VK_KHR_display", "VK_KHR_surface",
            "VK_EXT_direct_mode_display"};
#endif
		
#ifdef VK_USE_PLATFORM_WIN32_KHR
        std::vector<const char*> pRequiredExtentions = {"VK_KHR_win32_surface",
            "VK_KHR_surface"};
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        std::vector<const char*> pRequiredExtentions = {"VK_KHR_wayland_surface",
			"VK_KHR_display", "VK_EXT_direct_mode_display",
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

	VkDescriptorBufferInfo CVulkanRenderer::createDescriptorBufferInfo( VkBuffer ubo, const VkDeviceSize& uboStructSize, const VkDeviceSize& offsetStep ) {
		VkDescriptorBufferInfo uboBufferInfo{};
		uboBufferInfo.buffer = ubo;
		uboBufferInfo.offset = offsetStep * uboStructSize;
		uboBufferInfo.range = uboStructSize;

		return uboBufferInfo;
	}

	VkDescriptorImageInfo CVulkanRenderer::createDescriptorImageInfo( const VK_Image& textureImage, VkImageLayout layout, unsigned int textureViewIndex, VkSampler textureSampler ) {
		VkDescriptorImageInfo imageInfo{};
//		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageLayout = layout;
		imageInfo.imageView = textureImage.views[textureViewIndex];
		imageInfo.sampler = textureSampler;

		return imageInfo;
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
		// if ( pCallbackData->messageIdNumber == 941228658 ) {
		// 	[[maybe_unused]] int i = 0;
		// }


		
		// std::cout << "Error code: " << pCallbackData->messageIdNumber << std::endl;
		// std::cout << "Message name:: " << pCallbackData->pMessageIdName << std::endl;
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

	[[nodiscard]] mat4* CVulkanRenderer::updateAnimationFrames(ecs::components::transform* _transformComponent, unsigned int meshID) {
		if ( jointMatricesPerMesh.GetSize() > 0 && jointMatricesPerMesh[meshID].GetSize() > 0 &&
			 _transformComponent->frameAccumulator >= frames[meshID][_transformComponent->currentAnimationFrame] * 1.0f ) {
			++_transformComponent->currentAnimationFrame;
			if ( jointMatricesPerMesh[meshID].GetSize() > 0 && _transformComponent->currentAnimationFrame == frames[meshID].GetSize() ) {
				_transformComponent->currentAnimationFrame = 0;
				_transformComponent->frameAccumulator = 0.0f;
			}
		}

		unsigned int joinMatricesDataSize{};
		if ( jointMatricesPerMesh.GetSize() > 0 )
			joinMatricesDataSize = jointMatricesPerMesh[meshID].GetSize();

		mat4* jointMatricesData = nullptr;
		if ( joinMatricesDataSize == 0 ) {
			jointMatricesData = new mat4[MAX_JOINTS_NUMBER];
			for ( unsigned int i = 0; i < MAX_JOINTS_NUMBER; ++i ) {
				mat4 unitMatrix(1.0f);
				jointMatricesData[i] = unitMatrix;
			}
				
		} else {
			jointMatricesData = new mat4[MAX_JOINTS_NUMBER];
			for ( unsigned int i = 0; i < joinMatricesDataSize; ++i ) {
				jointMatricesData[i] = jointMatricesPerMesh[meshID][i][_transformComponent->currentAnimationFrame];
			}

			for ( u32 j = joinMatricesDataSize; j < MAX_JOINTS_NUMBER; ++j ) {
				mat4 unitMatrix(1.0f);
				jointMatricesData[j] = unitMatrix;
			}
		}

		return jointMatricesData;
	}

	mat4 CVulkanRenderer::computeModelMatrix(ecs::components::transform* _transformComponent) {
		mat4 rotationMatrix(1.0f);
        mat4 scalingMatrix(1.0f);
        mat4 translationMatrix(1.0f);
		
		scalingMatrix[0][0] = _transformComponent->scale;
		scalingMatrix[1][1] = _transformComponent->scale;
		scalingMatrix[2][2] = _transformComponent->scale;

		translationMatrix[3][0] = _transformComponent->position[0];
		translationMatrix[3][1] = _transformComponent->position[1];
		translationMatrix[3][2] = _transformComponent->position[2];
		translationMatrix[3][3] = 1.0f;

		float sinPitch = std::sin(Radians(-_transformComponent->pitch / 2));
		float cosPitch = std::cos(Radians(-_transformComponent->pitch / 2));
		float sinYaw = std::sin(Radians((_transformComponent->yaw)  / 2));
		float cosYaw = std::cos(Radians((_transformComponent->yaw)  / 2));
		
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

//		Quaternion result;
		// result = multiplyQuaternion(pitchQuat, yawQuat);


		// glm::quat rotation = glm::quat(cos(glm::radians(fPitch/2)),(glm::radians(fPitch/2))*1, 0,0);
		// glm::mat4 rotationMat = glm::mat4_cast(rotation);
		// // result = { rotation.w, rotation.x, rotation.y, rotation.z };
		// // rotationMatrix = rotateQuaternion<float, 4>(result);
		// for ( unsigned int i = 0; i < 4; ++i )
		// 	for ( unsigned int j = 0; j < 4; ++j )
		// 		rotationMatrix[i][j] = rotationMat[i][j];
		
        return scalingMatrix * translationMatrix;
	}
	
	void CVulkanRenderer::setImageDebugObjectName(VK_Image image) {
		VkDebugUtilsObjectNameInfoEXT imageObjectInfo{};
		imageObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string imageName = VK_DEBUG_IMAGE_SET_RED;
		const char* strImageName = imageName.c_str();
		imageObjectInfo.pObjectName = strImageName;
		imageObjectInfo.objectType = VK_OBJECT_TYPE_IMAGE;
		imageObjectInfo.objectHandle = (uint64_t)image.image;
		SetDebugObjectName(device, &imageObjectInfo);
	}

	void CVulkanRenderer::setPipelineDebugObjectName( VkPipeline pipeline, std::string pipelineName ) {
		VkDebugUtilsObjectNameInfoEXT mainPipelineObjectInfo{};
		mainPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string mainPipeLineImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_RED, " \x1b[31m" + pipelineName + " pipeline #\x1b[0m ", 0);
		const char* mainPipeLineStrImageName = mainPipeLineImageName.c_str();
		mainPipelineObjectInfo.pObjectName = mainPipeLineStrImageName;
		mainPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		mainPipelineObjectInfo.objectHandle = (uint64_t)pipeline;
		SetDebugObjectName(device, &mainPipelineObjectInfo);
	}

	void CVulkanRenderer::setDescriptorSetObjectName( VkDescriptorSet descriptorSet, std::string descriptorSetName, unsigned int index ) {
		VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
		descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " \x1b[31m" + descriptorSetName + " descriptor set #\x1b[0m ", index);
		const char* strName = name.c_str();
		descriptorSetObjectInfo.pObjectName = strName;
		descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
		descriptorSetObjectInfo.objectHandle = (uint64_t)descriptorSet;
		SetDebugObjectName(device, &descriptorSetObjectInfo);
	}
	
	void CVulkanRenderer::setDebugObjectNames() {
		setPipelineDebugObjectName( pipelineConfigs[SpecificPipeline::FONT_PIPELINE].pipeline, "fontPipeline" );
		setPipelineDebugObjectName( pipelineConfigs[SpecificPipeline::UI_PIPELINE].pipeline, "uiPipeline" );
		setPipelineDebugObjectName( pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].pipeline, "uiIconsPipeline" );
		
 		VkDebugUtilsObjectNameInfoEXT mainPipelineObjectInfo{};
		mainPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string mainPipeLineImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_RED, " \x1b[31mMain pipeline #\x1b[0m ", 0);
		const char* mainPipeLineStrImageName = mainPipeLineImageName.c_str();
		mainPipelineObjectInfo.pObjectName = mainPipeLineStrImageName;
		mainPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		mainPipelineObjectInfo.objectHandle = (uint64_t)pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipeline;
		SetDebugObjectName(device, &mainPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT mainPipelineLayoutObjectInfo{};
		mainPipelineLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string mainPipelineLayoutImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_LAYOUT_RED, " \x1b[31mMain pipeline layout #\x1b[0m ", 0);
		const char* mainPipelineLayoutStrImageName = mainPipelineLayoutImageName.c_str();
		mainPipelineLayoutObjectInfo.pObjectName = mainPipelineLayoutStrImageName;
		mainPipelineLayoutObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
		mainPipelineLayoutObjectInfo.objectHandle = (uint64_t)pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout;
		SetDebugObjectName(device, &mainPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT directionalLightPipelineObjectInfo{};
		directionalLightPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string directionalLightPipeLineImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_RED, " \x1b[31mDirectional light pipeline #\x1b[0m ", 0);
		const char* directionalLightPipeLineStrImageName = directionalLightPipeLineImageName.c_str();
		directionalLightPipelineObjectInfo.pObjectName = directionalLightPipeLineStrImageName;
		directionalLightPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		directionalLightPipelineObjectInfo.objectHandle = (uint64_t)pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].pipeline;
		SetDebugObjectName(device, &directionalLightPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT directionalLightPipelineLayoutObjectInfo{};
		directionalLightPipelineLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string directionalLightPipelineLayoutImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_LAYOUT_RED, " \x1b[31mDirectional light pipeline layout #\x1b[0m ", 0);
		const char* directionalLightPipelineLayoutStrImageName = directionalLightPipelineLayoutImageName.c_str();
		directionalLightPipelineLayoutObjectInfo.pObjectName = directionalLightPipelineLayoutStrImageName;
		directionalLightPipelineLayoutObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
		directionalLightPipelineLayoutObjectInfo.objectHandle = (uint64_t)pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].pipelineLayout;
		SetDebugObjectName(device, &directionalLightPipelineLayoutObjectInfo);

		VkDebugUtilsObjectNameInfoEXT spotLightPipelineObjectInfo{};
		spotLightPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string spotLightPipeLineImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_RED, " \x1b[31mSpot light pipeline #\x1b[0m ", 0);
		const char* spotLightPipeLineStrImageName = spotLightPipeLineImageName.c_str();
		spotLightPipelineObjectInfo.pObjectName = spotLightPipeLineStrImageName;
		spotLightPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		spotLightPipelineObjectInfo.objectHandle = (uint64_t)pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].pipeline;
		SetDebugObjectName(device, &spotLightPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT spotLightPipelineLayoutObjectInfo{};
		spotLightPipelineLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string spotLightPipelineLayoutImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_LAYOUT_RED, " \x1b[31mSpot light pipeline layout #\x1b[0m ", 0);
		const char* spotLightPipelineLayoutStrImageName = spotLightPipelineLayoutImageName.c_str();
		spotLightPipelineLayoutObjectInfo.pObjectName = spotLightPipelineLayoutStrImageName;
		spotLightPipelineLayoutObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
		spotLightPipelineLayoutObjectInfo.objectHandle = (uint64_t)pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].pipelineLayout;
		SetDebugObjectName(device, &spotLightPipelineLayoutObjectInfo);

		VkDebugUtilsObjectNameInfoEXT pointLightPipelineObjectInfo{};
		pointLightPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string pointLightPipeLineImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_RED, " \x1b[31mPoint light pipeline #\x1b[0m ", 0);
		const char* pointLightPipeLineStrImageName = pointLightPipeLineImageName.c_str();
		pointLightPipelineObjectInfo.pObjectName = pointLightPipeLineStrImageName;
		pointLightPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		pointLightPipelineObjectInfo.objectHandle = (uint64_t)pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].pipeline;
		SetDebugObjectName(device, &pointLightPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT pointLightPipelineLayoutObjectInfo{};
		pointLightPipelineLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string pointLightPipelineLayoutImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_PIPELINE_LAYOUT_RED, " \x1b[31mPoint light pipeline layout #\x1b[0m ", 0);
		const char* pointLightPipelineLayoutStrImageName = pointLightPipelineLayoutImageName.c_str();
		pointLightPipelineLayoutObjectInfo.pObjectName = pointLightPipelineLayoutStrImageName;
		pointLightPipelineLayoutObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
		pointLightPipelineLayoutObjectInfo.objectHandle = (uint64_t)pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].pipelineLayout;
		SetDebugObjectName(device, &pointLightPipelineLayoutObjectInfo);

		VkDebugUtilsObjectNameInfoEXT hudUniformBufferObjectInfo{};
		hudUniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string hudImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Hud uniform buffer # ", 0);
		const char* hudStrImageName = hudImageName.c_str();
		hudUniformBufferObjectInfo.pObjectName = hudStrImageName;
		hudUniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		unsigned int hudUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::HUD].descriptorsBindingsIDs[0];		
		hudUniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[descriptorBindingsConfig[hudUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
		SetDebugObjectName(device, &hudUniformBufferObjectInfo);

		VkDebugUtilsObjectNameInfoEXT fontUniformBufferObjectInfo{};
		fontUniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string fontImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Font uniform buffer # ", 0);
		const char* fontStrImageName = fontImageName.c_str();
		fontUniformBufferObjectInfo.pObjectName = fontStrImageName;
		fontUniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		unsigned int fontUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::FONT_RENDER_UBO].descriptorsBindingsIDs[0];
		fontUniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[descriptorBindingsConfig[fontUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;;
		SetDebugObjectName(device, &fontUniformBufferObjectInfo);

		VkDebugUtilsObjectNameInfoEXT uiUniformBufferObjectInfo{};
		uiUniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string uiImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " UI uniform buffer # ", 0);
		const char* uiStrImageName = uiImageName.c_str();
		uiUniformBufferObjectInfo.pObjectName = uiStrImageName;
		uiUniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		unsigned int uiUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::UI].descriptorsBindingsIDs[0];
		uiUniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[descriptorBindingsConfig[uiUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
		SetDebugObjectName(device, &uiUniformBufferObjectInfo);

		VkDebugUtilsObjectNameInfoEXT uiIconsUniformBufferObjectInfo{};
		uiIconsUniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string uiIconsImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " UI icons uniform buffer # ", 0);
		const char* uiIconsStrImageName = uiIconsImageName.c_str();
		uiIconsUniformBufferObjectInfo.pObjectName = uiIconsStrImageName;
		uiIconsUniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		unsigned int uiIconsUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::UI_ICONS].descriptorsBindingsIDs[0];		
		uiIconsUniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[descriptorBindingsConfig[uiIconsUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
		SetDebugObjectName(device, &uiIconsUniformBufferObjectInfo);

		VkDebugUtilsObjectNameInfoEXT directionalLightUniformBufferObjectInfo{};
		directionalLightUniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string directionalLightImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Shadow map directional light model matrix uniform buffer # ", 0);
		const char* directionalLightStrImageName = directionalLightImageName.c_str();
		directionalLightUniformBufferObjectInfo.pObjectName = directionalLightStrImageName;
		directionalLightUniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		unsigned int shadowMapDirectionalLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_DIRECTIONAL_LIGHT].descriptorsBindingsIDs[0];		
		directionalLightUniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[descriptorBindingsConfig[shadowMapDirectionalLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
		SetDebugObjectName(device, &directionalLightUniformBufferObjectInfo);

		VkDebugUtilsObjectNameInfoEXT pointLightUniformBufferObjectInfo{};
		pointLightUniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string pointLightImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Shadow map point light model matrix uniform buffer # ", 0);
		const char* pointLightStrImageName = pointLightImageName.c_str();
		pointLightUniformBufferObjectInfo.pObjectName = pointLightStrImageName;
		pointLightUniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		unsigned int shadowMapPointLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_POINT_LIGHT].descriptorsBindingsIDs[0];		
		pointLightUniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[descriptorBindingsConfig[shadowMapPointLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
		SetDebugObjectName(device, &pointLightUniformBufferObjectInfo);

		VkDebugUtilsObjectNameInfoEXT spotLightUniformBufferObjectInfo{};
		spotLightUniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string spotLightImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Shadow map spot light model matrix uniform buffer # ", 0);
		const char* spotLightStrImageName = spotLightImageName.c_str();
		spotLightUniformBufferObjectInfo.pObjectName = spotLightStrImageName;
		spotLightUniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		unsigned int shadowMapSpotLightDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_SPOT_LIGHT].descriptorsBindingsIDs[0];		
		spotLightUniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[descriptorBindingsConfig[shadowMapSpotLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
		SetDebugObjectName(device, &spotLightUniformBufferObjectInfo);

		for ( unsigned long i = 0; i < vertexBufferContainer.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT uniformBufferObjectInfo{};
			uniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string imageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Vertex uniform buffer # ", i);
			const char* strImageName = imageName.c_str();
			uniformBufferObjectInfo.pObjectName = strImageName;
			uniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
			uniformBufferObjectInfo.objectHandle = (uint64_t)vertexBufferContainer[i];
			SetDebugObjectName(device, &uniformBufferObjectInfo);
		}

		for ( unsigned long i = 0; i < indexBufferContainer.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT uniformBufferObjectInfo{};
			uniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string imageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Index uniform buffer # ", i);
			const char* strImageName = imageName.c_str();
			uniformBufferObjectInfo.pObjectName = strImageName;
			uniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
			uniformBufferObjectInfo.objectHandle = (uint64_t)indexBufferContainer[i];
			SetDebugObjectName(device, &uniformBufferObjectInfo);
		}

		for ( unsigned long i = 0; i < fontIndicesContainer.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT uniformBufferObjectInfo{};
			uniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string imageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Font vertex uniform buffer # ", fontIndicesContainer[i]);
			const char* strImageName = imageName.c_str();
			uniformBufferObjectInfo.pObjectName = strImageName;
			uniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
			uniformBufferObjectInfo.objectHandle = (uint64_t)fontVertexBufferContainer[fontIndicesContainer[i]];
			SetDebugObjectName(device, &uniformBufferObjectInfo);
		}

		for ( unsigned long i = 0; i < fontIndicesContainer.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT uniformBufferObjectInfo{};
			uniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string imageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Font index uniform buffer # ", fontIndicesContainer[i]);
			const char* strImageName = imageName.c_str();
			uniformBufferObjectInfo.pObjectName = strImageName;
			uniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
			uniformBufferObjectInfo.objectHandle = (uint64_t)fontIndexBufferContainer[fontIndicesContainer[i]];
			SetDebugObjectName(device, &uniformBufferObjectInfo);
		}

		VkDebugUtilsObjectNameInfoEXT uniformBufferObjectInfo{};
		uniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string imageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Model matrix uniform buffer # ", 0);
		const char* strImageName = imageName.c_str();
		uniformBufferObjectInfo.pObjectName = strImageName;
		uniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		uniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[DescriptorSetDataLink::MAIN_RENDER_MATRIX_UBO].GPUBuffer->buffer;
		SetDebugObjectName(device, &uniformBufferObjectInfo);

		VkDebugUtilsObjectNameInfoEXT lightDataUniformBufferObjectInfo{};
		lightDataUniformBufferObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		std::string lightDataImageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Light data uniform buffer # ", 0);
		const char* lightDataStrImageName = lightDataImageName.c_str();
		lightDataUniformBufferObjectInfo.pObjectName = lightDataStrImageName;
		lightDataUniformBufferObjectInfo.objectType = VK_OBJECT_TYPE_BUFFER;
		unsigned int lightDataUboDescriptorBindingIndex = descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[0];
		lightDataUniformBufferObjectInfo.objectHandle = (uint64_t)GPUDescriptors[descriptorBindingsConfig[lightDataUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer;
		SetDebugObjectName(device, &lightDataUniformBufferObjectInfo);

		// for ( unsigned long i = 0; i < directionalLightPipeline.descriptors[0].textureImages.size(); ++i ) {
		// 	VkDebugUtilsObjectNameInfoEXT directionalLightImageObjectInfo{};
		// 	directionalLightImageObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		// 	std::string imageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Directional light shadow map image # ", i);
		// 	const char* strImageName = imageName.c_str();
		// 	directionalLightImageObjectInfo.pObjectName = strImageName;
		// 	directionalLightImageObjectInfo.objectType = VK_OBJECT_TYPE_IMAGE;
		// 	directionalLightImageObjectInfo.objectHandle = (uint64_t)directionalLightPipeline.descriptors[0].textureImages[i].image;
		// 	SetDebugObjectName(device, &directionalLightImageObjectInfo);			
		// }

		// for ( unsigned long i = 0; i < textureImages.size(); ++i ) {
		// 	VkDebugUtilsObjectNameInfoEXT textureImageObjectInfo{};
		// 	textureImageObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		// 	std::string imageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " Texture image # ", i);
		// 	const char* strImageName = imageName.c_str();
		// 	textureImageObjectInfo.pObjectName = strImageName;
		// 	textureImageObjectInfo.objectType = VK_OBJECT_TYPE_IMAGE;
		// 	textureImageObjectInfo.objectHandle = (uint64_t)textureImages[i].image;
		// 	SetDebugObjectName(device, &textureImageObjectInfo);			
		// }

		// for ( unsigned long i = 0; i < swapChainImages.size(); ++i ) {
		// 	VkDebugUtilsObjectNameInfoEXT swapChainImageObjectInfo{};
		// 	swapChainImageObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		// 	std::string imageName = ConcatIntBetweenTwoStrings(VK_DEBUG_IMAGE_SET_RED, " SwapChain image # ", i);
		// 	const char* strImageName = imageName.c_str();
		// 	swapChainImageObjectInfo.pObjectName = strImageName;
		// 	swapChainImageObjectInfo.objectType = VK_OBJECT_TYPE_IMAGE;
		// 	swapChainImageObjectInfo.objectHandle = (uint64_t)swapChainImages[i];
		// 	SetDebugObjectName(device, &swapChainImageObjectInfo);			
		// }
		
		// for ( unsigned long i = 0; i < directionalLightPipeline.descriptors.GetSize(); ++i ) {
		// 	VkDebugUtilsObjectNameInfoEXT descriptorSetLayoutObjectInfo{};
		// 	descriptorSetLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		// 	std::string layoutName = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_LAYOUT_RED, " Directional light shadow map descriptor set layout # ", i);
		// 	const char* strLayoutName = layoutName.c_str();
		// 	descriptorSetLayoutObjectInfo.pObjectName = strLayoutName;
		// 	descriptorSetLayoutObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
		// 	descriptorSetLayoutObjectInfo.objectHandle = (uint64_t)directionalLightPipeline.descriptors[i].setLayout;
		// 	SetDebugObjectName(device, &descriptorSetLayoutObjectInfo);			
		// }

		// for ( unsigned long i = 0; i < mainRenderScenePipeline.descriptors.GetSize(); ++i ) {
		// 	VkDebugUtilsObjectNameInfoEXT descriptorSetLayoutObjectInfo{};
		// 	descriptorSetLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		// 	std::string layoutName = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_LAYOUT_RED, " Main render descriptor set layout # ", i);
		// 	const char* strLayoutName = layoutName.c_str();
		// 	descriptorSetLayoutObjectInfo.pObjectName = strLayoutName;
		// 	descriptorSetLayoutObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
		// 	descriptorSetLayoutObjectInfo.objectHandle = (uint64_t)mainRenderScenePipeline.descriptors[i].setLayout;
		// 	SetDebugObjectName(device, &descriptorSetLayoutObjectInfo);			
		// }
		

		
		// for ( unsigned long i = 0; i < viewPositionUboDescriptorSets.size(); ++i ) {
		// 	VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
		// 	descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		// 	std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render view position descriptor set # ", i);
		// 	const char* strName = name.c_str();
		// 	descriptorSetObjectInfo.pObjectName = strName;
		// 	descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
		// 	descriptorSetObjectInfo.objectHandle = (uint64_t)viewPositionUboDescriptorSets[i];
		// 	SetDebugObjectName(device, &descriptorSetObjectInfo);
		// }

		// for ( unsigned long i = 0; i < shadowMapDirectionalLightDescriptorSets.size(); ++i ) {
		// 	VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
		// 	descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		// 	std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render shadow map directional light descriptor set # ", i);
		// 	const char* strName = name.c_str();
		// 	descriptorSetObjectInfo.pObjectName = strName;
		// 	descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
		// 	descriptorSetObjectInfo.objectHandle = (uint64_t)shadowMapDirectionalLightDescriptorSets[i];
		// 	SetDebugObjectName(device, &descriptorSetObjectInfo);
		// }
	}
}

