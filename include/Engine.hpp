// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef ENGINE_HPP
#define ENGINE_HPP

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
#include <cstdint>
#include <mutex>
#include "TextureManager.hpp"
#include "Network/UDP_ServerLinux.hpp"
#include "VkStructs.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/LevelChunkArchetype.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "Archetypes/RigidBodyArchetype.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/EnemyArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Archetypes/StaticMeshArchetype.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "Archetypes/DirectionalLightArchetype.hpp"
#include "Archetypes/PointLightArchetype.hpp"
#include "Archetypes/SpotLightArchetype.hpp"

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

		// struct PreinitializedRenderData {
			
		// } preinitializedRenderData;

		/// PreinitializedRenderData
		ecs::arch::Archetype* cachedDirectionalLigthArchetypes[32];
		uint32_t directionalLightArchetypesNumber = 0;
		ecs::arch::componentMask directionalLightRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::DIRECTIONAL_LIGHT_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT)              |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT);

		ecs::arch::Archetype* cachedSpotLigthArchetypes[32];
		uint32_t spotLightArchetypesNumber = 0;
		ecs::arch::componentMask spotLightRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::SPOT_LIGHT_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT)       |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT);

		ecs::arch::Archetype* cachedPointLigthArchetypes[32];
		uint32_t pointLightArchetypesNumber = 0;
		ecs::arch::componentMask pointLightRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::POINT_LIGHT_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT)        |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT);

		ecs::arch::Archetype* cachedAnimationActorsArchetypes[32];
		uint32_t animationActorsArchetypesNumber = 0;
		ecs::arch::componentMask animatedActorsRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::MATERIAL_COMPONENT)  |
			(1ul << ecs::arch::ComponentsIndices::ANIMATION_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::ROTATION_COMPONENT)  |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT);

		ecs::arch::Archetype* cachedStaticActorsArchetypes[32];
		uint32_t staticActorsArchetypesNumber = 0;
		ecs::arch::componentMask staticActorsRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::MATERIAL_COMPONENT)        |
			(1ul << ecs::arch::ComponentsIndices::STATIC_MESH_TAG_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT)       |
			(1ul << ecs::arch::ComponentsIndices::ROTATION_COMPONENT)        |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT);



		
		
		ecs::arch::Archetype* cachedPlayerArchetypes[32];
		uint32_t playerArchetypesNumber = 0;
		ecs::arch::componentMask playerRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::PLAYER_TAG_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT)  |
			(1ul << ecs::arch::ComponentsIndices::VIEW_COMPONENT);


		
		ecs::arch::Archetype* cachedAnimationArchetypes[32];
		uint32_t animationArchetypesNumber = 0;
		ecs::arch::componentMask animationRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::MATERIAL_COMPONENT)  |
			(1ul << ecs::arch::ComponentsIndices::ANIMATION_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::ROTATION_COMPONENT)  |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT);

		
		ecs::arch::Archetype* cachedCrosshairActorsArchetypes[32];
		uint32_t crosshairActorsArchetypesNumber = 0;
		ecs::arch::componentMask crosshairRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::CROSSHAIR_TAG_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT)          |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT);

		ecs::arch::Archetype* cachedLevelChunkActorsArchetypes[32];
		uint32_t levelChunkActorsArchetypesNumber = 0;
		ecs::arch::componentMask levelChunkRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::MATERIAL_COMPONENT)        |
			(1ul << ecs::arch::ComponentsIndices::LEVEL_CHUNK_TAG_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT)       |
			(1ul << ecs::arch::ComponentsIndices::ROTATION_COMPONENT)        |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT);

		
		ecs::arch::Archetype* cachedProjectileActorsArchetypes[32];
		uint32_t projectileActorsArchetypesNumber = 0;
		ecs::arch::componentMask projectileRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT)         |
			(1ul << ecs::arch::ComponentsIndices::ROTATION_COMPONENT)          |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT);

		
		ecs::arch::Archetype* cachedItemActorsArchetypes[32];
		uint32_t itemActorsArchetypesNumber = 0;
		ecs::arch::componentMask rotationItemRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::ITEM_COMPONENT)           |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT)           |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT)      |
			(1ul << ecs::arch::ComponentsIndices::COLLIDER_COMPONENT)       |
			(1ul << ecs::arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::ROTATION_COMPONENT)       |
			(1ul << ecs::arch::ComponentsIndices::MATERIAL_COMPONENT);


		ecs::arch::Archetype* cachedInventoryArchetypes[32];
		uint32_t inventoryArchetypesNumber = 0;
		ecs::arch::componentMask inventoryRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::INVENTORY_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT)      |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MATERIAL_COMPONENT);

		
		ecs::arch::Archetype* cachedItemArchetypes[32];
		uint32_t itemArchetypesNumber = 0;
		ecs::arch::componentMask itemRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::ITEM_COMPONENT)           |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT)           |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT)      |
			(1ul << ecs::arch::ComponentsIndices::COLLIDER_COMPONENT)       |
			(1ul << ecs::arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MATERIAL_COMPONENT);
		
		ecs::arch::Archetype* cachedHealthBarsArchetypes[32];
		uint32_t healthBarsArchetypesNumber = 0;
		ecs::arch::componentMask healthBarsRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::HEALTH_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::MESH_COMPONENT)   |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT);

		ecs::arch::Archetype* cachedFontsArchetypes[32];
		uint32_t fontsArchetypesNumber = 0;
		ecs::arch::componentMask fontRequiredMask =
			(1ul << ecs::arch::ComponentsIndices::FONT_COMPONENT) |
			(1ul << ecs::arch::ComponentsIndices::TRANSFORM_COMPONENT);

		
		/// For FPS counting
		unsigned int fpsCounter = 0;
		double fpsAccumulator   = 0;

        Engine();
        
	public:
		core::vector<ecs::components::MeshHandle> meshHandlers;
		core::vector<ecs::TextureHandle> textureHandlers;
		uint32_t wavefrontObjCounter = 0;
        
        ~Engine();
        
        Engine(Engine& _engine) = delete;                   ///< Dont need to make copy because of singleton property.
        void operator=(const Engine& _engine) = delete;      ///< Dont need assignment operator because of singleton property.
        static Engine* GetInstance();                        ///< It possibly to get only one instance of this class whith this method
        
		void GameLoop();
		void EventQueueFlush();
		void RenderVulkan();
		void EnlargeFrameAccumulator(float value);
		void SetViewMatrix();
		void SetProjectionMatrix();
		[[nodiscard]] core::vector<mat4> updateAnimationFrames(ecs::components::animation* animationComponent, unsigned int meshID);
		mat4 updateDirectionalLightSpaceMatrixShadowMapUBO(ecs::components::directionalLight* directionalLightComponent);
		mat4 updateSpotLightSpaceMatrixShadowMapUBO( ecs::components::spotLight* spotLightComponent );
		mat4 updatePointLightSpaceMatrixShadowMapUBO( ecs::components::pointLight* pointLightComponent, uint32_t layer );
		SlotData updateDataUBO_UI(const unsigned int currentInventoryRow, const unsigned int currentInventoryColumn,
								  ecs::components::inventory* inventoryComponent,
								  ecs::components::transform* slotTransfromComponent,
								  ecs::components::mesh*      meshComponent);
		mat4 updateDataUBO_IconsUI(ecs::components::transform* itemTransfromComponent,
								   [[maybe_unused]] ecs::components::collider* itemColliderComponent,
								   ecs::components::item* itemComponent,
								   const unsigned int rowInventory,
								   const unsigned int columnInventory,
								   ecs::components::transform* inventoryTransformComponent,
								   ecs::components::mesh* itemMesh,
								   int itemEntity);
		mat4 updateDataHudScreenUBO( ecs::components::transform* cursorTransform );
		void setFrameData();
		void loadWavefrontObj();
		void initializeGLTF();
		void initializeFontData();
		mat4 computeModelMatrix(ecs::components::transform* _transformComponent, ecs::components::rotation* rotation);
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
