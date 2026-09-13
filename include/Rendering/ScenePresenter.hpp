#pragma once
#include "Rendering/RenderAssets.hpp"
#include "Rendering/RenderFrame.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "ComponentsFullSet.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Event.hpp"
#include "EventsStack.hpp"
namespace GLVM::core {
// Adapts the ECS to CPU render data. It does not own GPU objects or call Vulkan.
class ScenePresenter final {
public:
    ScenePresenter(ecs::arch::World& world, const RenderAssets& assets,
                   RenderFrame& frame, CEvent& event, CStack& input);
    void advanceAnimations(float seconds);
    void updatePointer();
    void prepareFrame();
private:
    ecs::arch::World& world_;
    const RenderAssets& assets_;
    RenderFrame& frame_;
    CEvent& event_;
    CStack& input_;
    vec3 previousFrameForward = {0.0f, 0.0f, -1.0f};
    int previousFrameKeyEvents[4] = {};
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
		mat4 computeModelMatrix(ecs::components::transform* _transformComponent, ecs::components::rotation* rotation);
		void computeHudScreeenCoordinates();
};
}
