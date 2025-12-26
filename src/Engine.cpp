// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Engine.hpp"
#include "Components/MaterialComponent.hpp"
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
			vulkanRenderer->EnlargeFrameAccumulator(deltaFrameTime);
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
			vulkanRenderer->draw();
			vulkanRenderer->Window->SwapBuffers();
		}

//		vulkanRenderer->Window->Close();
		delete vulkanRenderer;
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

	void Engine::setFrameData() {
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
		namespace cm = GLVM::ecs::components;
		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
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
