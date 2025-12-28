// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef ENGINE
#define ENGINE

#include "Components/InventoryComponent.hpp"
#include "ComponentsFullSet.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "ProceduralLevelGeneratingSystem.hpp"
#include "ShaderStructs.hpp"
#include "Systems/DamageSystem.hpp"
#include "Systems/EnemySystem.hpp"
#include "Systems/InventorySystem.hpp"
#include "Systems/ItemSystem.hpp"
#include "SystemsFullSet.hpp"
#include "IChrono.hpp"
#include "IWindow.hpp"
#include "ISoundEngine.hpp"
#include "EventsStack.hpp"
#include "Event.hpp"
#include "Texture.hpp"
#include "TimerCreator.hpp"
#include "Vector.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"
#include "IContainer.hpp"
#include "Constants.hpp"
#include <mutex>
#include "TextureManager.hpp"
#include "Network/UDP_ServerLinux.hpp"
#include "VkStructs.hpp"

using Entity = unsigned int;

namespace GLVM::core
{
	enum RendererType {
		OPENGL_RENDERER,
		VULKAN_RENDERER
	};
	
	class Engine
	{
        static Engine*    pInstance_;
        static std::mutex  Mutex_;
        
		Time::IChrono       * chrono;
        Sound::ISoundEngine * soundEngine;
		float                deltaFrameTime;
		float                gravity;
//		CStack               Input_Stack_;
		bool                 isLeftMouseButtonPressed;
		std::vector<ecs::Texture> textureVector;
		std::vector<const char*> pathsArray_;
		core::vector<const char*> pathsGLTF_;
		uint32_t meshID = 0;
		float hud_screen_x = 0.0f;
		float hud_screen_y;
		int                  dragedItemEntity = -1;             ///< If dont have any draged item then this variable have value of -1
		[[maybe_unused]] float fYaw   = -90.0f;
        [[maybe_unused]] float fPitch = 0.0f;
		[[maybe_unused]] float previousMouseOffsetX = 0.0f;
		[[maybe_unused]] float previousMouseOffsetY = 0.0f;

		CVulkanRenderer*     vulkanRenderer;

//		ecs::CSystemManager* pSystem_Manager;
		
        ecs::CCollisionSystem           * collisionSystem;
		ecs::CMovementSystem            * movementSystem;
        ecs::CPhysicsSystem             * physicsSystem;
        ecs::CProjectileSystem          * projectileSystem;
		ecs::DamageSystem               * damageSystem;
		ecs::EnemySystem                * enemySytem;
		ecs::ItemSystem                 * itemSystem;
		ProceduralLevelGeneratingSystem * procuduralLevelGeneratingSystem;
		ecs::InventorySystem            * inventorySystem;

		/// For FPS counting
		unsigned int fpsCounter = 0;
		double fpsAccumulator   = 0;

        Engine();
        
	public:
		core::vector<ecs::components::MeshHandle> meshHandlers;
		core::vector<ecs::TextureHandle> textureHandlers;
        
        ~Engine();
        
        Engine(Engine& _engine) = delete;                   ///< Dont need to make copy because of singleton property.
        void operator=(const Engine& _engine) = delete;      ///< Dont need assignment operator because of singleton property.
        static Engine* GetInstance();                        ///< It possibly to get only one instance of this class whith this method
        
		void GameLoop();
		void EventQueueFlush();
		void RenderVulkan();
		void EnlargeFrameAccumulator(float value);
		[[nodiscard]] core::vector<mat4> updateAnimationFrames(ecs::components::transform* _transformComponent, unsigned int meshID);
		mat4 updateDirectionalLightSpaceMatrixShadowMapUBO(ecs::components::directionalLight* directionalLightComponent);
		mat4 updateSpotLightSpaceMatrixShadowMapUBO( ecs::components::spotLight* spotLightComponent );
		mat4 updatePointLightSpaceMatrixShadowMapUBO( ecs::components::pointLight* pointLightComponent, uint32_t layer );
		SlotData updateDataUBO_UI(const unsigned int currentInventoryRow, const unsigned int currentInventoryColumn,
							  ecs::components::inventory* inventoryComponent,
								  ecs::components::transform* slotTransfromComponent);
		mat4 updateDataUBO_IconsUI(ecs::components::transform* itemTransfromComponent,
							   [[maybe_unused]] ecs::components::collider* itemColliderComponent,
							   ecs::components::item* itemComponent,
							   const unsigned int rowInventory,
							   const unsigned int columnInventory,
							   ecs::components::transform* inventoryTransformComponent,
							   int itemEntity);
		mat4 updateDataHudScreenUBO( ecs::components::transform* cursorTransform );
		void setFrameData();
		mat4 computeModelMatrix(ecs::components::transform* _transformComponent);
		void computeHudScreeenCoordinates();
		ecs::TextureHandle LoadTextureFromFile(const char* path_to_texture);
		ecs::TextureHandle LoadTextureFromAddress(unsigned int iWidth, unsigned int iHeight,
								  unsigned int dat_length, unsigned char* u_iData);
		ecs::components::MeshHandle LoadMeshFromFile_OBJ(const char* _pathToMesh);
		ecs::components::MeshHandle LoadMeshFromFile_GLTF(const char* pathToMesh);
		ecs::components::MeshHandle LoadMesh();
		void FPScounter();
		void GameKill();
	};
}

#endif
