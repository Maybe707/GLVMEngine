// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Engine.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Event.hpp"
#include "ISoundEngine.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "ISoundEngine.hpp"
#include "ProceduralLevelGeneratingSystem.hpp"
#include "ShaderStructs.hpp"
#include "SoundEngineFactory.hpp"
#include "SystemManager.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/DamageSystem.hpp"
#include "Systems/EnemySystem.hpp"
#include "Systems/InventorySystem.hpp"
#include "Systems/ItemSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/ProjectileSystem.hpp"
#include "Texture.hpp"
#include "VkStructs.hpp"
#include <cstdint>
#include <limits>
#include <mutex>
#include <sys/types.h>
#include <thread>
#include <wayland-client-core.h>


/*******************************************************************
 * Legends never die...
 * You are about to face most terrifying data structures of all time.
 *    "Abandon hope all ye who enter here..." (c) Dante Alighieri.
 *******************************************************************
 *****************  👑  !!!  DESTRUCTOR_3000  !!!  👑  *************/

/*******************************************************************
*                                                                  *
*                             \_/                                  *
*                            (* *)                                 *
*                           __)#(__                                *
*                          ( )...( )(_)                            *
*                          || |_| ||//                             *
*                       >==() | | ()/                              *
*                           _(___)_                                *
*                          [-]   [-]                               *
*                                                                  *
********************************************************************/

#define DESTRUCTOR_3000													\
    std::cout << "You have been destructurized. [=]___[=]" << std::endl; \
    exit(1)

GLVM::core::CEvent g_eEvent;
GLVM::core::vector<GLVM::core::MeshAxisMaxAbsoluteValues> allMeshMaxAbsoluteValues;      /// contain all maximum absolute axis values
// struct wl_surface*    wl_surface;
// struct wl_compositor* compositor;
// struct xdg_toplevel*  xdg_topLevel;
// struct xdg_wm_base*   xdg_shell;
// struct wl_buffer*     buffer;
// struct wl_shm*        shared_memory;
// struct wl_seat*       seat;
// struct wl_keyboard*   keyboard;
// void* pixels;
// uint16_t width = 480;
// uint16_t height = 320;
// uint8_t  constant_byte = 0;
// uint8_t  close_xdg_toplevel;
// struct wl_display*  display;
// struct wl_registry* registry;
// struct wl_callback* frame_callback;
// struct xdg_surface* xdg_surface;

namespace GLVM::core
{
    Engine* Engine::pInstance_ = nullptr;
    std::mutex Engine::Mutex_;

    void PlaybackSound(Sound::ISoundEngine* _sound_Engine) {
        while(1) {
			_sound_Engine->SoundStream();
		}
    }

    Engine::Engine() {
		chrono                          = Time::CTimerCreator().Create();
		soundEngine                     = Sound::CSoundEngineFactory().CreateSoundEngine();
								        
		collisionSystem                 = new ecs::CCollisionSystem(Input_Stack_);
		movementSystem                  = new ecs::CMovementSystem(Input_Stack_);
		physicsSystem                   = new ecs::CPhysicsSystem(gravity, Input_Stack_);
		projectileSystem                = new ecs::CProjectileSystem(Input_Stack_);
		damageSystem                    = new ecs::DamageSystem();
		enemySytem                      = new ecs::EnemySystem();
		itemSystem                      = new ecs::ItemSystem();
		procuduralLevelGeneratingSystem = new ProceduralLevelGeneratingSystem();
		inventorySystem                 = new ecs::InventorySystem();
        
		deltaFrameTime             = 0.0;
		g_eEvent.SetEvent(eDEFAULT);

		ecs::CSystemManager* pSystem_Manager = ecs::CSystemManager::GetInstance();

		///< Call of ActivateSystem function must be in this order.
		pSystem_Manager->ActivateSystem(procuduralLevelGeneratingSystem);
		pSystem_Manager->ActivateSystem(movementSystem);
		pSystem_Manager->ActivateSystem(enemySytem);
		pSystem_Manager->ActivateSystem(projectileSystem);
		pSystem_Manager->ActivateSystem(collisionSystem);
		pSystem_Manager->ActivateSystem(damageSystem);
		pSystem_Manager->ActivateSystem(physicsSystem);
		pSystem_Manager->ActivateSystem(inventorySystem);
		pSystem_Manager->ActivateSystem(itemSystem);

		// std::thread sound_thread(PlaybackSound, std::ref(soundEngine));
		// sound_thread.detach();
    }
	
    Engine::~Engine() {}
            
    Engine* Engine::GetInstance() {
		std::lock_guard<std::mutex> lock(Mutex_);
		if(pInstance_ == nullptr) {
			pInstance_ = new Engine();
		}
		return pInstance_;
    }

    void Engine::GameLoop() {
		RenderVulkan();
    }

	void Engine::EventQueueFlush() {
	}
	
	void Engine::RenderVulkan() {
		ecs::CSystemManager* pSystem_Manager = ecs::CSystemManager::GetInstance();
		bool bGame_Loop_Active = true;

		projectileSystem->textureHandlers = textureHandlers;
		projectileSystem->meshHandlers    = meshHandlers;

		enemySytem->textureHandlers       = textureHandlers;
		enemySytem->meshHandlers          = meshHandlers;

		procuduralLevelGeneratingSystem->meshHandlers    = meshHandlers;
		procuduralLevelGeneratingSystem->textureHandlers = textureHandlers;

		inventorySystem->isItemDraged     = &dragedItemEntity;
		itemSystem->dragedItemEntity      = &dragedItemEntity;
		
		vulkanRenderer = new CVulkanRenderer();
		vulkanRenderer->initializeTextureData_ = textureVector;
		vulkanRenderer->pathsArray_            = pathsArray_;
		vulkanRenderer->pathsGLTF_             = pathsGLTF_;
		GLVM::core::MeshManager*   meshManager = GLVM::core::MeshManager::GetInstance();
		vulkanRenderer->SetMeshData(meshManager->pathsArray_, meshManager->pathsGLTF_);
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager   = ecs::ComponentManager::GetInstance();
		core::vector<Entity> directionalLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																									  cm::directionalLight,
																									  cm::mesh>();
		vulkanRenderer->directionalLightNumber = directionalLightLinkedEntities.GetSize();
		core::vector<Entity> spotLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							   cm::spotLight,
																							   cm::mesh>();
		vulkanRenderer->spotLightNumber = spotLightLinkedEntities.GetSize();
		core::vector<Entity> pointLightLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																								cm::pointLight,
																								cm::mesh>();
		vulkanRenderer->pointLightNumber = pointLightLinkedEntities.GetSize();
		core::vector<Entity> actorsLinkedEntities = componentManager->collectLinkedEntities<cm::transform,
																							cm::material,
																							cm::mesh>();
		vulkanRenderer->actorsNumber = actorsLinkedEntities.GetSize();
		loadWavefrontObj();
		initializeGLTF();
		vulkanRenderer->run();
//		vulkanRenderer->Window->Input_Stack_    = &Input_Stack_;		

#ifdef __linux__
		// XEvent uXEvent;
		// while (XPending(vulkanRenderer->Window.GetDisplay())) {
		// 	XNextEvent(vulkanRenderer->Window.GetDisplay(), &uXEvent);
		// }

		// xcb_generic_event_t* event;
		// while (( event = xcb_poll_for_event ( vulkanRenderer->Window.GetConnection() ))) {
		// }
#endif

#ifdef _WIN32
		MSG msg;

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
		}

		// while(GetMessageA(&msg, vulkanRenderer->Window.GetModernWindowHWND(), WM_KEYFIRST, WM_KEYLAST)) {
		// 		// TranslateMessage( &msg );
		// 		// DispatchMessage( &msg );
		// }
#endif

		while(bGame_Loop_Active) {
			deltaFrameTime = chrono->GetElapsed();
			chrono->Reset();
		    gravity += deltaFrameTime;

			vulkanRenderer->Window->ClearDisplay();
             
			vulkanRenderer->Window->HandleEvent(g_eEvent);
//			std::cout << "left mouse released flag" << g_eEvent.isItemDraged << std::endl;			
			// 	Input_Stack_.ControlInput(g_eEvent);
			if((Input_Stack_.SearchElement(EEvents::eGAME_LOOP_KILL)) == EEvents::eGAME_LOOP_KILL) {
				bGame_Loop_Active = false;
			}

			if((Input_Stack_.SearchElement(EEvents::eMOUSE_LEFT_BUTTON)) == EEvents::eMOUSE_LEFT_BUTTON) {
				isLeftMouseButtonPressed = true;
			} else {
				isLeftMouseButtonPressed = false;
			}

			if((Input_Stack_.SearchElement(EEvents::eINVENTORY)) == EEvents::eINVENTORY) {
				vulkanRenderer->isInventoryOpened = !vulkanRenderer->isInventoryOpened;
				Input_Stack_.Remove(EEvents::eINVENTORY);
				if ( vulkanRenderer->isInventoryOpened )
					pSystem_Manager->DeactivateSystem(ecs::DeactivatedSystems::DEACTIVATED_MOVEMENT_SYSTEM);
				else
					pSystem_Manager->ReturnSystemToActivatedState(ecs::DeactivatedSystems::DEACTIVATED_MOVEMENT_SYSTEM);
//				bGame_Loop_Active = false;
			}
			// }
			g_eEvent.SetLastEvent(Input_Stack_);

			vulkanRenderer->Window->CursorLock(g_eEvent.mousePointerPosition.position_X,
								  g_eEvent.mousePointerPosition.position_Y,
								  &g_eEvent.mousePointerPosition.offset_X,
											  &g_eEvent.mousePointerPosition.offset_Y);

			computeHudScreeenCoordinates();
			// std::cout << "lmb released " << g_eEvent.isLeftMouseButtonReleased << std::endl;
			// std::cout << "lmb pressed " << isLeftMouseButtonPressed << std::endl;
			// std::cout << "item draged " << itemSystem->isItemDraged << std::endl;

			damageSystem->deltaTime                   = deltaFrameTime;
			movementSystem->deltaFrameTime            = deltaFrameTime;
			movementSystem->gravity                   = gravity;
			collisionSystem->fDelta_Time_             = deltaFrameTime;
			collisionSystem->gravity                  = gravity;
			collisionSystem->isInventoryOpened        = vulkanRenderer->isInventoryOpened;
//			collisionSystem->isItemDraged             = &itemSystem->isItemDraged;
			collisionSystem->isLeftMouseButtonPressed = isLeftMouseButtonPressed;
			collisionSystem->isLeftMouseButtonReleased = &g_eEvent.isLeftMouseButtonReleased;
			enemySytem->deltaFrameTime                = deltaFrameTime;
			enemySytem->soundEngine                   = soundEngine;
			projectileSystem->deltaFrameTime          = deltaFrameTime;
			projectileSystem->soundEngine             = soundEngine;
			projectileSystem->isInventoryOpened       = vulkanRenderer->isInventoryOpened;
			physicsSystem->fDelta_Time_               = deltaFrameTime;
			physicsSystem->fAcceleration_of_Gravity_ += (deltaFrameTime / 20);
			physicsSystem->gravity                    = gravity;
			inventorySystem->isInventoryOpened         = vulkanRenderer->isInventoryOpened;
			inventorySystem->isLeftMouseButtonReleased = &g_eEvent.isLeftMouseButtonReleased;
			inventorySystem->isLeftMouseButtonPressed  = isLeftMouseButtonPressed;
			inventorySystem->mouseOffsetX              = hud_screen_x;
			inventorySystem->mouseOffsetY              = hud_screen_y;
			itemSystem->inputStack                    = &Input_Stack_;
			itemSystem->isInventoryOpened             = vulkanRenderer->isInventoryOpened;
			itemSystem->isLeftMouseButtonReleased     = &g_eEvent.isLeftMouseButtonReleased;
			itemSystem->isLeftMouseButtonPressed      = isLeftMouseButtonPressed;
			itemSystem->mouseOffsetX                  = hud_screen_x;
			itemSystem->mouseOffsetY                  = hud_screen_y;
			EnlargeFrameAccumulator(deltaFrameTime);
			pSystem_Manager->Update();
			vulkanRenderer->levelGeneratedVertices    = procuduralLevelGeneratingSystem->levelGeneratedVertices;
			vulkanRenderer->levelGeneratedIndices     = procuduralLevelGeneratingSystem->levelGeneratedIndices;
			procuduralLevelGeneratingSystem->levelGeneratedVertices.clear();
			procuduralLevelGeneratingSystem->levelGeneratedIndices.clear();
			vulkanRenderer->dragedItemEntity          = dragedItemEntity;
			vulkanRenderer->hud_screen_x              = hud_screen_x;
			vulkanRenderer->hud_screen_y              = hud_screen_y;
			vulkanRenderer->initializeGameLevelVertices();
			setFrameData();
			SetViewMatrix();
			SetProjectionMatrix();
			vulkanRenderer->draw();
			vulkanRenderer->Window->SwapBuffers();
		}

//		vulkanRenderer->Window->Close();
		delete vulkanRenderer;
	}

	void Engine::EnlargeFrameAccumulator(float value) {
		namespace cm = GLVM::ecs::components;
		
		ecs::ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::animation, cm::transform, cm::mesh>();
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
		for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
			Entity currentEntity                = linkedEntities[i];
			cm::transform* transformComponent   = componentManager->GetComponent<cm::transform>(currentEntity);
			unsigned int mesh_id                = componentManager->GetComponent<cm::mesh>(currentEntity)->handle.id;

			if ( vulkanRenderer->jointMatricesPerMesh.GetSize() > 0 && vulkanRenderer->jointMatricesPerMesh[mesh_id].GetSize() > 0 )
				transformComponent->frameAccumulator += value;
		}
	}

    void Engine::SetViewMatrix()
    {
		namespace cm = GLVM::ecs::components;

		ecs::ComponentManager* componentManager = GLVM::ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::beholder>();
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
		cm::beholder* cameraComponent   = nullptr;
		cm::transform* _Player          = nullptr;
		for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
			Entity currentEntity                = linkedEntities[i];
			cameraComponent     = componentManager->GetComponent<cm::beholder>(currentEntity);
			_Player             = componentManager->GetComponent<cm::transform>(currentEntity);
		}

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

		vulkanRenderer->current_X = (float)g_eEvent.mousePointerPosition.offset_X;
		vulkanRenderer->current_Y = (float)g_eEvent.mousePointerPosition.offset_Y;
		float delta_x = 0.0f;
		float delta_y = 0.0f;
		#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		delta_x = vulkanRenderer->current_X;
		delta_y = vulkanRenderer->current_Y;
		#else
		delta_x = current_X - prev_X;
		delta_y = current_Y - prev_Y;
		delta_y *= -1.0f;
		#endif
		// delta_x *= kSensitivity;
		// delta_y *= kSensitivity;
		
		const vec3 rightVec = Cross(cameraComponent->forward, cameraComponent->up);
		const vec3 newUpVec = Cross(rightVec, cameraComponent->forward);
		/*
		 * 1. The mouse direction determines the "intended direction of rotation" for the object.
		 * 2. The camera is "looking forward."
		 * 3. To make the object "rotate as if the mouse is pushing it," you need to rotate it around an axis that is perpendicular to both the view direction and the mouse movement.
		 */
		const vec3 rotateAxis = Normalize(Cross(cameraComponent->forward, rightVec * delta_x + newUpVec * delta_y));

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
					.ry = -rotateAxis.m_vector[1], .rz = -rotateAxis.m_vector[2]}) >> pga::point{ .x = cameraComponent->forward[0],
					.y = cameraComponent->forward[1], .z = cameraComponent->forward[2], .w = 1.0f };
			vulkanRenderer->forward[0] = appliedRotationPoint.x;
			vulkanRenderer->forward[1] = appliedRotationPoint.y;
			vulkanRenderer->forward[2] = appliedRotationPoint.z;
		}
		cameraComponent->forward = Normalize(vulkanRenderer->forward);
		_Player->forward = cameraComponent->forward;
		mat4 view = LookAtMain(cameraComponent->Position + _Player->position, cameraComponent->Position + _Player->position + cameraComponent->forward, cameraComponent->up);
		for ( unsigned int i = 0; i < 4; ++i )
			for ( unsigned int j = 0; j < 4; ++j )
				 viewMatrix_[i][j] = view[i][j];
		
		if ( !vulkanRenderer->isInventoryOpened )
			vulkanRenderer->viewMatrix = viewMatrix_;

		vulkanRenderer->prev_Y = (float)g_eEvent.mousePointerPosition.offset_Y;
		vulkanRenderer->prev_X = (float)g_eEvent.mousePointerPosition.offset_X;
    }
	
	void Engine::SetProjectionMatrix()
	{
		mat4 tProjection_Matrix = Perspective(Radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
		vulkanRenderer->projectionMatrix = tProjection_Matrix;
		vulkanRenderer->projectionMatrix[1][1] *= 1.0f;
	}
	
	[[nodiscard]] core::vector<mat4> Engine::updateAnimationFrames(ecs::components::transform* _transformComponent, unsigned int meshID) {
		if ( vulkanRenderer->jointMatricesPerMesh.GetSize() > 0 && vulkanRenderer->jointMatricesPerMesh[meshID].GetSize() > 0 &&
			 _transformComponent->frameAccumulator >= vulkanRenderer->frames[meshID][_transformComponent->currentAnimationFrame] * 1.0f ) {
			++_transformComponent->currentAnimationFrame;
			if ( vulkanRenderer->jointMatricesPerMesh[meshID].GetSize() > 0 && _transformComponent->currentAnimationFrame == vulkanRenderer->frames[meshID].GetSize() ) {
				_transformComponent->currentAnimationFrame = 0;
				_transformComponent->frameAccumulator = 0.0f;
			}
		}

		unsigned int joinMatricesDataSize{};
		if ( vulkanRenderer->jointMatricesPerMesh.GetSize() > 0 )
			joinMatricesDataSize = vulkanRenderer->jointMatricesPerMesh[meshID].GetSize();

		core::vector<mat4> jointMatrices;
		if ( joinMatricesDataSize == 0 ) {
			jointMatrices.Resize(MAX_JOINTS_NUMBER);
			for ( unsigned int i = 0; i < MAX_JOINTS_NUMBER; ++i ) {
				mat4 unitMatrix(1.0f);
				jointMatrices[i] = unitMatrix;
			}
				
		} else {
			jointMatrices.Resize(MAX_JOINTS_NUMBER);
			for ( unsigned int i = 0; i < joinMatricesDataSize; ++i ) {
				jointMatrices[i] = vulkanRenderer->jointMatricesPerMesh[meshID][i][_transformComponent->currentAnimationFrame];
			}

			for ( u32 j = joinMatricesDataSize; j < MAX_JOINTS_NUMBER; ++j ) {
				mat4 unitMatrix(1.0f);
				jointMatrices[j] = unitMatrix;
			}
		}

		return jointMatrices;
	}

	mat4 Engine::updateDirectionalLightSpaceMatrixShadowMapUBO( ecs::components::directionalLight* directionalLightComponent ) {
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
		return viewMatrixLight * directionalProjectionMatrixLight;
	}

	mat4 Engine::updateSpotLightSpaceMatrixShadowMapUBO( ecs::components::spotLight* spotLightComponent ) {
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
		return viewMatrixLight * spotProjectionMatrixLight;
	}

	mat4 Engine::updatePointLightSpaceMatrixShadowMapUBO( ecs::components::pointLight* pointLightComponent, uint32_t layer ) {
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

		return viewMatrixLight * projectionMatrixCubeShadowMap;
	}

	[[nodiscard]] SlotData Engine::updateDataUBO_UI(const unsigned int currentInventoryRow, const unsigned int currentInventoryColumn,
								  ecs::components::inventory* inventoryComponent,
								  ecs::components::transform* slotTransfromComponent) {
		SlotData hudUBO{};
		mat4 model(1.0);
		const float fullSlotScale     = slotTransfromComponent->gltf ? inventoryComponent->slotScale * 2.0f : inventoryComponent->slotScale;
		const float x = slotTransfromComponent->position[0] + currentInventoryColumn * fullSlotScale;
		const float y_scaleMultilayer = vulkanRenderer->aspectRate * fullSlotScale;
		const float y = slotTransfromComponent->position[1] + currentInventoryRow * y_scaleMultilayer;
		const float inventorySlotScale = inventoryComponent->slotScale;
		model[0][0] = inventorySlotScale;
		model[1][1] = inventorySlotScale;
		model[2][2] = inventorySlotScale;
		model[3][0] = x;
		model[3][1] = y;
		model[3][2] = 0.1f;
		
		hudUBO.model = model;

		bool highLightedSlot = false;
		for ( unsigned int i = 0; i < inventoryComponent->highlightedSlots.GetSize(); ++i ) {
			if ( inventoryComponent->highlightedSlots[i] == currentInventoryRow * inventoryComponent->col + currentInventoryColumn ) {
				highLightedSlot = true;
				break;
			} else
				continue;
		}

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

		return hudUBO;
	}

	mat4 Engine::updateDataUBO_IconsUI(ecs::components::transform* itemTransfromComponent,
								   [[maybe_unused]] ecs::components::collider* itemColliderComponent,
								   ecs::components::item* itemComponent,
								   const unsigned int rowInventory,
								   const unsigned int columnInventory,
								   ecs::components::transform* inventoryTransformComponent,
								   int itemEntity) {
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
			y_result_offset = inventoryTransformComponent->position[1] + (rowIndexFirstSlot * fullSlotScale + rowIndexSecondSlot * fullSlotScale) * centreMultiplayer * vulkanRenderer->aspectRate;
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

		return model;
	}

	mat4 Engine::updateDataHudScreenUBO( ecs::components::transform* cursorTransform ) {
		mat4 model;
		vec3 defaultPosition = vec3(0.0, 0.0, 0.0);

#ifndef VK_USE_PLATFORM_WAYLAND_KHR
		hud_screen_x = -hud_screen_x;
#endif
		
		cursorTransform->position[0] = hud_screen_x;
		cursorTransform->position[1] = -hud_screen_y;
//		std::cout << "cursor scale: " << cursorTransform->fScale << std::endl;

//		std::cout << "x: " << cursorTransform->tPosition[0] << " y: " << cursorTransform->tPosition << std::endl;
		
		if ( !vulkanRenderer->isInventoryOpened ) {
			model[3][0] = defaultPosition[0];
			model[3][1] = defaultPosition[1];
			model[3][2] = defaultPosition[2];
			model[0][0] = cursorTransform->scale;
			model[1][1] = cursorTransform->scale;
			model[2][2] = cursorTransform->scale;
			model[3][3] = 1.0f;
		} else {
			defaultPosition[0] = hud_screen_x;
			defaultPosition[1] = -hud_screen_y;
			
			model[3][0] = defaultPosition[0];
			model[3][1] = defaultPosition[1];
			model[3][2] = defaultPosition[2];
			model[0][0] = cursorTransform->scale;
			model[1][1] = cursorTransform->scale;
			model[2][2] = cursorTransform->scale;
			model[3][3] = 1.0f;
		}

		return model;
	}
	
	void Engine::setFrameData() {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
		namespace cm = GLVM::ecs::components;
		core::vector<Entity> directionalLightEntities      = componentManager->collectLinkedEntities<cm::transform,
																									 cm::directionalLight,
																									 cm::mesh,
																									 cm::actor>();
		vulkanRenderer->directionalLights.clear();
		for ( uint32_t directionalLightCounter = 0; directionalLightCounter < directionalLightEntities.GetSize(); ++ directionalLightCounter ) {
			unsigned int directionalLightEntity = directionalLightEntities[directionalLightCounter];
			vulkanRenderer->directionalLights.Push({});
			cm::directionalLight* directionalLightComponent = componentManager->GetComponent<cm::directionalLight>( directionalLightEntity );
			vulkanRenderer->directionalLights[directionalLightCounter].DirectionalLightSpaceMatrix =
				updateDirectionalLightSpaceMatrixShadowMapUBO( directionalLightComponent );
			vulkanRenderer->directionalLights[directionalLightCounter].position = vec4(directionalLightComponent->position[0],
																					   directionalLightComponent->position[1],
																					   directionalLightComponent->position[2], 0.0);
			vulkanRenderer->directionalLights[directionalLightCounter].direction = vec4(directionalLightComponent->direction[0],
																						directionalLightComponent->direction[1],
																						directionalLightComponent->direction[2], 0.0);
			vulkanRenderer->directionalLights[directionalLightCounter].ambient = vec4(directionalLightComponent->ambient[0],
																					  directionalLightComponent->ambient[1],
																					  directionalLightComponent->ambient[2], 0.0);
			vulkanRenderer->directionalLights[directionalLightCounter].diffuse = vec4(directionalLightComponent->diffuse[0],
																					  directionalLightComponent->diffuse[1],
																					  directionalLightComponent->diffuse[2], 0.0);
			vulkanRenderer->directionalLights[directionalLightCounter].specular = vec4(directionalLightComponent->specular[0],
																					   directionalLightComponent->specular[1],
																					   directionalLightComponent->specular[2], 0.0);
		}

		core::vector<Entity> spotLightEntities      = componentManager->collectLinkedEntities<cm::transform,
																							  cm::spotLight,
																							  cm::mesh,
																							  cm::actor>();
		vulkanRenderer->spotLights.clear();
		for ( uint32_t spotLightCounter = 0; spotLightCounter < spotLightEntities.GetSize(); ++ spotLightCounter ) {
			unsigned int spotLightEntity = spotLightEntities[spotLightCounter];
			vulkanRenderer->spotLights.Push({});
			cm::spotLight* spotLightComponent = componentManager->GetComponent<cm::spotLight>( spotLightEntity );
			vulkanRenderer->spotLights[spotLightCounter].SpotLigthSpaceMatrix =
				updateSpotLightSpaceMatrixShadowMapUBO( spotLightComponent );
			vulkanRenderer->spotLights[spotLightCounter].position    = spotLightComponent->position;
			vulkanRenderer->spotLights[spotLightCounter].direction   = spotLightComponent->direction;
			vulkanRenderer->spotLights[spotLightCounter].cutOff      = spotLightComponent->cutOff;
			vulkanRenderer->spotLights[spotLightCounter].outerCutOff = spotLightComponent->outerCutOff;
			vulkanRenderer->spotLights[spotLightCounter].ambient     = spotLightComponent->ambient;
			vulkanRenderer->spotLights[spotLightCounter].diffuse     = spotLightComponent->diffuse;
			vulkanRenderer->spotLights[spotLightCounter].specular    = spotLightComponent->specular;
			vulkanRenderer->spotLights[spotLightCounter].constant    = spotLightComponent->constant;
			vulkanRenderer->spotLights[spotLightCounter].linear      = spotLightComponent->linear;
			vulkanRenderer->spotLights[spotLightCounter].quadratic   = spotLightComponent->quadratic;
		}

		core::vector<Entity> pointLightEntities = componentManager->collectLinkedEntities<cm::transform,
																						  cm::pointLight,
																						  cm::mesh,
																						  cm::actor>();

		vulkanRenderer->pointLights.clear();
		for ( uint32_t pointLightCounter = 0; pointLightCounter < pointLightEntities.GetSize(); ++pointLightCounter ) {
			unsigned int pointLightEntity = pointLightEntities[pointLightCounter];
			vulkanRenderer->pointLights.Push({});
			cm::pointLight* pointLightComponent = componentManager->GetComponent<cm::pointLight>(pointLightEntity);
			uint32_t maxCubeMapLayers = 6;
			for ( uint32_t cubeMapLayerCounter = 0; cubeMapLayerCounter < maxCubeMapLayers; ++cubeMapLayerCounter ) {                      ///< 6 is a number of cube map layers.
				vulkanRenderer->pointLights[pointLightCounter].pointLightSpaceMatrix[cubeMapLayerCounter] =
					updatePointLightSpaceMatrixShadowMapUBO( pointLightComponent, cubeMapLayerCounter );
			}
			vulkanRenderer->pointLights[pointLightCounter].position  = pointLightComponent->position;
			vulkanRenderer->pointLights[pointLightCounter].ambient   = pointLightComponent->ambient;
			vulkanRenderer->pointLights[pointLightCounter].diffuse   = pointLightComponent->diffuse;
			vulkanRenderer->pointLights[pointLightCounter].specular  = pointLightComponent->specular;
			vulkanRenderer->pointLights[pointLightCounter].constant  = pointLightComponent->constant;
			vulkanRenderer->pointLights[pointLightCounter].linear    = pointLightComponent->linear;
			vulkanRenderer->pointLights[pointLightCounter].quadratic = pointLightComponent->quadratic;
		}

		namespace cm = GLVM::ecs::components;
		core::vector<Entity> healthEntities = componentManager->collectLinkedEntities<cm::transform,
																					  cm::health>();

		vulkanRenderer->healthBars.clear();
		for ( unsigned int i = 0; i < healthEntities.GetSize(); ++i ) {
			unsigned int uiEntity = healthEntities[i];
			vulkanRenderer->healthBars.Push({});
			unsigned int uiVertexId = componentManager->GetComponent<ecs::components::mesh>(0)->handle.id;
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(uiEntity);
			cm::health* healthComponent = componentManager->GetComponent<cm::health>(uiEntity);
			vulkanRenderer->healthBars[i].meshID        = uiVertexId;
			vulkanRenderer->healthBars[i].position      = transformComponent->position;
			vulkanRenderer->healthBars[i].maxHealth     = healthComponent->maxHealth;
			vulkanRenderer->healthBars[i].currentHealth = healthComponent->currentHealth;
		}

		core::vector<Entity> fontEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::font>();
		vulkanRenderer->fonts.clear();
		for ( unsigned int i = 0; i < fontEntities.GetSize(); ++i ) {
			unsigned int entity = fontEntities[i];
			vulkanRenderer->fonts.Push({});
			cm::font* fontComponent = componentManager->GetComponent<cm::font>(entity);
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(entity);
			vulkanRenderer->fonts[i].position    = transformComponent->position;
			vulkanRenderer->fonts[i].font_string = fontComponent->font_string;
			vulkanRenderer->fonts[i].lifeTime    = fontComponent->lifeTime;
		}

		if ( vulkanRenderer->isInventoryOpened ) {
			core::vector<Entity> inventoryEntities      = componentManager->collectLinkedEntities<cm::inventory>();
			vulkanRenderer->inventories.clear();
			for ( unsigned int i = 0; i < inventoryEntities.GetSize(); ++i ) {
				unsigned int uiEntity = inventoryEntities[i];
				vulkanRenderer->inventories.Push({});
				cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(uiEntity);
				unsigned int inventoryTextureID   = componentManager->GetComponent<cm::material>(uiEntity)->diffuseTextureID_.id;
				unsigned int meshID           = inventoryComponent->slotMeshID.id;
				vulkanRenderer->inventories[i].inventoryTextureID = inventoryTextureID;
				vulkanRenderer->inventories[i].meshID             = meshID;
				vulkanRenderer->inventories[i].row                = inventoryComponent->row;
				vulkanRenderer->inventories[i].col                = inventoryComponent->col;
				vulkanRenderer->inventories[i].slotData.clear();
				for ( unsigned int j = 0; j < inventoryComponent->row; ++j ) {
					for ( unsigned int m = 0; m < inventoryComponent->col; ++m ) {
						cm::transform* slotTransformComponent     = componentManager->GetComponent<cm::transform>(uiEntity);
						vulkanRenderer->inventories[i].slotData.Push({});
						vulkanRenderer->inventories[i].slotData[j * inventoryComponent->col + m] =
							updateDataUBO_UI( j, m, inventoryComponent, slotTransformComponent );
					}
				}
			}

			cm::inventory* inventoryComponent = componentManager->GetComponent<cm::inventory>(inventoryEntities[0]);
			cm::transform* inventoryTransformComponent = componentManager->GetComponent<cm::transform>(inventoryEntities[0]);
		
			core::vector<Entity> itemEntities = componentManager->collectLinkedEntities<cm::mesh, cm::material, cm::transform, cm::collider, cm::item>();
			vulkanRenderer->items.clear();
			for ( unsigned int i = 0; i < itemEntities.GetSize(); ++i ) {
				unsigned int itemEntity = itemEntities[i];
				vulkanRenderer->items.Push({});
				cm::item* itemComponent = componentManager->GetComponent<cm::item>(itemEntity);
				cm::actor* actorComponent = componentManager->GetComponent<cm::actor>(itemEntity);
				if ( actorComponent != nullptr )
					continue;
			
				unsigned int meshID = componentManager->GetComponent<ecs::components::mesh>(itemEntity)->handle.id;
				unsigned int diffuseTexureID = componentManager->GetComponent<ecs::components::material>(itemEntity)->diffuseTextureID_.id;
				vulkanRenderer->items[i].meshID          = meshID;
				vulkanRenderer->items[i].diffuseTexureID = diffuseTexureID;
				cm::transform* itemTransformComponent = componentManager->GetComponent<cm::transform>(itemEntity);
				cm::collider* itemColliderComponent = componentManager->GetComponent<cm::collider>(itemEntity);

				if ( itemTransformComponent == nullptr )
					std::cout << "NULL POINTER" << std::endl;

				vulkanRenderer->items[i].model = updateDataUBO_IconsUI(itemTransformComponent,
																	   itemColliderComponent,
																	   itemComponent,
																	   inventoryComponent->row,
																	   inventoryComponent->col,
																	   inventoryTransformComponent,
																	   itemEntity);
			}
		}

		core::vector<Entity> crosshairEntities = componentManager->collectLinkedEntities<cm::crosshair, cm::transform>();
		vulkanRenderer->crosshairs.clear();
		for ( unsigned int i = 0; i < crosshairEntities.GetSize(); ++i ) {
			unsigned int uiEntity = crosshairEntities[i];
			vulkanRenderer->crosshairs.Push({});
			cm::transform* cursorTransform = componentManager->GetComponent<ecs::components::transform>(uiEntity);
			unsigned int meshID = componentManager->GetComponent<ecs::components::mesh>(uiEntity)->handle.id;
			vulkanRenderer->crosshairs[i].meshID = meshID;
			vulkanRenderer->crosshairs[i].model  = updateDataHudScreenUBO( cursorTransform );
		}
		
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::transform,
																						   cm::material,
																						   cm::mesh,
																						   cm::actor>();
		vulkanRenderer->actors.clear();
		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			unsigned int uiEntity = linkedEntities[i];
			vulkanRenderer->actors.Push({});
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(uiEntity);
			cm::material*  materialComponent  = componentManager->GetComponent<cm::material>(uiEntity);
			unsigned int meshID = componentManager->GetComponent<ecs::components::mesh>(uiEntity)->handle.id;
			vulkanRenderer->actors[i].modelMatrix   = computeModelMatrix(transformComponent);
			vulkanRenderer->actors[i].jointMatrices = updateAnimationFrames(transformComponent, meshID);
			vulkanRenderer->actors[i].meshID        = meshID;
			vulkanRenderer->actors[i].diffuseTextureIndex  = materialComponent->diffuseTextureID_.id;
			vulkanRenderer->actors[i].specularTextureIndex = materialComponent->specularTextureID_.id;
			vulkanRenderer->actors[i].ambient   = materialComponent->ambient;
			vulkanRenderer->actors[i].shininess = materialComponent->shininess;
		}

		core::vector<Entity> playerEntities = componentManager->collectLinkedEntities<cm::beholder, cm::transform>();
		vulkanRenderer->players.clear();
		for( unsigned int i = 0; i < playerEntities.GetSize(); ++i ) {
			unsigned int entityID = playerEntities[i];
			vulkanRenderer->players.Push({});
			cm::transform* playerTransformComponent = componentManager->GetComponent<cm::transform>(entityID);
			vulkanRenderer->players[i].position = playerTransformComponent->position;
			vulkanRenderer->players[i].forward  = playerTransformComponent->forward;
		}
	}

    void Engine::loadWavefrontObj() {
        for (unsigned int m = 0; m < pathsArray_.size(); ++m) {
            CWaveFrontObjParser parser;
            CWaveFrontObjParser* wavefrontObjParser = &parser;
            
            wavefrontObjParser->ReadFile(pathsArray_[m]);
            wavefrontObjParser->ParseFile();

            vulkanRenderer->aIndices_.emplace_back();
            vulkanRenderer->aVertices_.emplace_back();
			vulkanRenderer->highest_gltf_Y.emplace_back();
			vulkanRenderer->highest_gltf_Y[m] = -999.999f;

			vulkanRenderer->frames.Push({});
			vulkanRenderer->jointMatricesPerMesh.Push({});
			allMeshMaxAbsoluteValues.Push({});
            
            unsigned int vertexIndex  = 0;
            unsigned int textureIndex = 0;
			unsigned int normalIndex  = 0;
            unsigned int faceVerticesSize = wavefrontObjParser->getFaces().GetSize();

			vulkanRenderer->meshAxisLimitingValues.highest_x = -MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.lowest_x  = MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.highest_y = -MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.lowest_y  = MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.highest_z = -MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.lowest_z  = MAXFLOAT;
			
            for (unsigned int i = 0; i < faceVerticesSize; ++i)
                for (int j = 0; j < 3; ++j) {
                    vertexIndex     = wavefrontObjParser->getFaces()[i][0][j] - 1;
					vulkanRenderer->aIndices_[m].push_back(i * 3 + j);
                    SVertex vertex  = wavefrontObjParser->getCoordinateVertices()[vertexIndex];
                    textureIndex    = wavefrontObjParser->getFaces()[i][1][j] - 1;
                    SVertex texture = wavefrontObjParser->getTextureVertices()[textureIndex];
					normalIndex     = wavefrontObjParser->getFaces()[i][2][j] - 1;
					SVertex normal  = wavefrontObjParser->getNormals()[normalIndex];

					vec4 jointIndices;
					vec4 weights;

					if ( vertex[1] > vulkanRenderer->highest_gltf_Y[m] )
						vulkanRenderer->highest_gltf_Y[m] = vertex[1];

					if ( vertex[0] < vulkanRenderer->meshAxisLimitingValues.lowest_x ) {
						vulkanRenderer->meshAxisLimitingValues.lowest_x = vertex[0];
					} else if ( vertex[0] > vulkanRenderer->meshAxisLimitingValues.highest_x ) {
						vulkanRenderer->meshAxisLimitingValues.highest_x = vertex[0];
					}

					if ( vertex[1] < vulkanRenderer->meshAxisLimitingValues.lowest_y ) {
						vulkanRenderer->meshAxisLimitingValues.lowest_y = vertex[1];
					} else if ( vertex[1] > vulkanRenderer->meshAxisLimitingValues.highest_y ) {
						vulkanRenderer->meshAxisLimitingValues.highest_y = vertex[1];
					}

					if ( vertex[2] < vulkanRenderer->meshAxisLimitingValues.lowest_z ) {
						vulkanRenderer->meshAxisLimitingValues.lowest_z = vertex[2];
					} else if ( vertex[2] > vulkanRenderer->meshAxisLimitingValues.highest_z ) {
						vulkanRenderer->meshAxisLimitingValues.highest_z = vertex[2];
					}
					
					jointIndices[0] = -1;
					jointIndices[1] = -1;
					jointIndices[2] = -1;
					jointIndices[3] = -1;

					weights[0] = 1.0f;
					weights[1] = 1.0f;
					weights[2] = 1.0f;
					weights[2] = 1.0f;
					
                    vulkanRenderer->aVertices_[m].Push({{vertex[0], vertex[1], vertex[2]},
										{normal[0], normal[1], normal[2]},
										{texture[0], texture[1]},
										{jointIndices[0], jointIndices[1], jointIndices[2]},
										{weights[0], weights[1], weights[2]}});
                }
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x = (vulkanRenderer->meshAxisLimitingValues.highest_x - vulkanRenderer->meshAxisLimitingValues.lowest_x) / 2.0f;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y = (vulkanRenderer->meshAxisLimitingValues.highest_y - vulkanRenderer->meshAxisLimitingValues.lowest_y) / 2.0f;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z = (vulkanRenderer->meshAxisLimitingValues.highest_z - vulkanRenderer->meshAxisLimitingValues.lowest_z) / 2.0f;
			++wavefrontObjCounter;
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
        }
    }

	void Engine::initializeGLTF() {
		core::vector<bool> animationFlags;
		for (unsigned int m = 0; m < pathsGLTF_.GetSize(); ++m) {
			Core::CJsonParser jsonParser;
			vulkanRenderer->aVertexesTemp_.emplace_back();
			vulkanRenderer->aIndices_.emplace_back();
			vulkanRenderer->frames.Push({});
			vulkanRenderer->jointMatricesPerMesh.Push({});
			animationFlags.Push({});
			vulkanRenderer->highest_gltf_Y.emplace_back();
			uint32_t nextIndexGLTF = wavefrontObjCounter + m;
			jsonParser.LoadGLTF(pathsGLTF_[m], vulkanRenderer->aVertexesTemp_[m], vulkanRenderer->aIndices_[nextIndexGLTF],
								vulkanRenderer->jointMatricesPerMesh[nextIndexGLTF], vulkanRenderer->frames[nextIndexGLTF],
								animationFlags[m], vulkanRenderer->highest_gltf_Y[nextIndexGLTF]);
		}

		for (unsigned int m = 0; m < pathsGLTF_.GetSize(); ++m) {
//            aIndices_.emplace_back();
//            aVertices_.emplace_back();
			vulkanRenderer->aVertices_.emplace_back();
			allMeshMaxAbsoluteValues.Push({});

			vulkanRenderer->meshAxisLimitingValues.highest_x = -MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.lowest_x  = MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.highest_y = -MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.lowest_y  = MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.highest_z = -MAXFLOAT;
			vulkanRenderer->meshAxisLimitingValues.lowest_z  = MAXFLOAT;
			
			int stepOffset = 0;
			if ( animationFlags[m] )
				stepOffset = 8;
			else
				stepOffset = 16;
			for ( unsigned int n = 0; n < vulkanRenderer->aVertexesTemp_[m].size(); n += stepOffset ) {
				SVertex vertex;
				vertex[0] = vulkanRenderer->aVertexesTemp_[m][n];
			    vertex[1] = vulkanRenderer->aVertexesTemp_[m][n + 1];
				vertex[2] = vulkanRenderer->aVertexesTemp_[m][n + 2];
				SVertex normal;
				normal[0] = vulkanRenderer->aVertexesTemp_[m][n + 3];
				normal[1] = vulkanRenderer->aVertexesTemp_[m][n + 4];
				normal[2] = vulkanRenderer->aVertexesTemp_[m][n + 5];
				SVertex texture;
				texture[0] = vulkanRenderer->aVertexesTemp_[m][n + 6];
				texture[1] = vulkanRenderer->aVertexesTemp_[m][n + 7];

				if ( vertex[0] < vulkanRenderer->meshAxisLimitingValues.lowest_x ) {
					vulkanRenderer->meshAxisLimitingValues.lowest_x = vertex[0];
				} else if ( vertex[0] > vulkanRenderer->meshAxisLimitingValues.highest_x ) {
					vulkanRenderer->meshAxisLimitingValues.highest_x = vertex[0];
				}

				if ( vertex[1] < vulkanRenderer->meshAxisLimitingValues.lowest_y ) {
					vulkanRenderer->meshAxisLimitingValues.lowest_y = vertex[1];
				} else if ( vertex[1] > vulkanRenderer->meshAxisLimitingValues.highest_y ) {
					vulkanRenderer->meshAxisLimitingValues.highest_y = vertex[1];
				}

				if ( vertex[2] < vulkanRenderer->meshAxisLimitingValues.lowest_z ) {
					vulkanRenderer->meshAxisLimitingValues.lowest_z = vertex[2];
				} else if ( vertex[2] > vulkanRenderer->meshAxisLimitingValues.highest_z ) {
					vulkanRenderer->meshAxisLimitingValues.highest_z = vertex[2];
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
					joinIndices[0] = vulkanRenderer->aVertexesTemp_[m][n + 8];
					joinIndices[1] = vulkanRenderer->aVertexesTemp_[m][n + 9];
					joinIndices[2] = vulkanRenderer->aVertexesTemp_[m][n + 10];
					joinIndices[3] = vulkanRenderer->aVertexesTemp_[m][n + 11];

					weights[0] = vulkanRenderer->aVertexesTemp_[m][n + 12];
					weights[1] = vulkanRenderer->aVertexesTemp_[m][n + 13];
					weights[2] = vulkanRenderer->aVertexesTemp_[m][n + 14];
					weights[3] = vulkanRenderer->aVertexesTemp_[m][n + 15];
				}

				uint32_t nextIndexGLTF = wavefrontObjCounter + m;
				vulkanRenderer->aVertices_[nextIndexGLTF].Push({{vertex[0], vertex[1], vertex[2]},
										 {normal[0], normal[1], normal[2]},
										 {texture[0], texture[1]},
										 {joinIndices[0], joinIndices[1], joinIndices[2], joinIndices[3]},
										 {weights[0], weights[1], weights[2], weights[3]}});
			}
//			uint32_t nextIndexGLTF = vulkanRenderer->wavefrontObjCounter + m;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x = (vulkanRenderer->meshAxisLimitingValues.highest_x - vulkanRenderer->meshAxisLimitingValues.lowest_x) / 2.0f;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y = (vulkanRenderer->meshAxisLimitingValues.highest_y - vulkanRenderer->meshAxisLimitingValues.lowest_y) / 2.0f;
			allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z = (vulkanRenderer->meshAxisLimitingValues.highest_z - vulkanRenderer->meshAxisLimitingValues.lowest_z) / 2.0f;

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
		}
	}
	
	mat4 Engine::computeModelMatrix(ecs::components::transform* _transformComponent) {
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
	
	void Engine::computeHudScreeenCoordinates() {
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		hud_screen_y -= g_eEvent.mousePointerPosition.offset_Y / 1080.0f;
		hud_screen_x += g_eEvent.mousePointerPosition.offset_X / 1920.0f;
#else
		hud_screen_y -= (previousMouseOffsetY - g_eEvent.mousePointerPosition.offset_Y) / 1080.0f;
		hud_screen_x += (previousMouseOffsetX - g_eEvent.mousePointerPosition.offset_X) / 1920.0f;
		previousMouseOffsetX = g_eEvent.mousePointerPosition.offset_X;
		previousMouseOffsetY = g_eEvent.mousePointerPosition.offset_Y;
#endif

		if ( hud_screen_x > 1.0f )
			hud_screen_x = 1.0f;
		else if ( hud_screen_x < -1.0f )
			hud_screen_x = -1.0f;
		
		if ( hud_screen_y > 1.0f )
			hud_screen_y = 1.0f;
		else if ( hud_screen_y < -1.0f )
			hud_screen_y = -1.0f;
	}
	
	ecs::TextureHandle Engine::LoadTextureFromFile(const char* path_to_texture) {
		uint32_t textureID = textureVector.size();
		ecs::TextureHandle textureHandle;
		textureHandle.id = textureID;
		textureVector.push_back({ .path_to_image = path_to_texture });
		textureHandlers.Push(textureHandle);

		return textureHandle;
	}
	
	ecs::TextureHandle Engine::LoadTextureFromAddress(unsigned int iWidth, unsigned int iHeight,
								  unsigned int dat_length, unsigned char* u_iData) {
		uint32_t textureID = textureVector.size();
		ecs::TextureHandle textureHandle;
		textureHandle.id = textureID;
		textureVector.push_back({ .iWidth_ = iWidth, .iHeight_ = iHeight, .dat_length_ = dat_length, .u_iData_ = u_iData});
		textureHandlers.Push(textureHandle);

		return textureHandle;
    }

	ecs::components::MeshHandle Engine::LoadMeshFromFile_OBJ(const char* _pathToMesh) {
		ecs::components::MeshHandle meshHandle;
		meshHandle.id = meshID;
        pathsArray_.push_back(_pathToMesh);
		meshHandlers.Push(meshHandle);
		++meshID;

		return meshHandle;
    }

	ecs::components::MeshHandle Engine::LoadMeshFromFile_GLTF(const char* pathToMesh) {
		ecs::components::MeshHandle meshHandle;
		meshHandle.id = meshID;
        pathsGLTF_.Push(pathToMesh);
		meshHandlers.Push(meshHandle);
		++meshID;

		return meshHandle;
	}

	ecs::components::MeshHandle Engine::LoadMesh() {
		ecs::components::MeshHandle meshHandle;
		meshHandle.id = meshID;
		meshHandlers.Push(meshHandle);
		++meshID;

		return meshHandle;
	}
	
	void Engine::FPScounter() {
		++fpsCounter;
		fpsAccumulator += deltaFrameTime;
		if (fpsAccumulator > 1.0f) {
			std::cout << "FPS: " << fpsCounter << std::endl;
			fpsCounter = 0;
			fpsAccumulator = 0;
		}
	}
	
    void Engine::GameKill()
    {
		// delete soundEngine;
		// soundEngine = nullptr;
		delete chrono;
		chrono = nullptr;
		delete collisionSystem;
		collisionSystem = nullptr;
		delete movementSystem;
		movementSystem = nullptr;
		delete physicsSystem;
		physicsSystem = nullptr;
		delete projectileSystem;
		projectileSystem = nullptr;
		delete damageSystem;
		damageSystem = nullptr;
		delete enemySytem;
		enemySytem = nullptr;
		delete itemSystem;
		itemSystem = nullptr;
		// delete pSystem_Manager;
		// pSystem_Manager = nullptr;
    }
} // namespace GLVM::core
