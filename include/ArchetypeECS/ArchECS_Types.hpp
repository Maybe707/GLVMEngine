#ifndef ARCH_ECS_TYPES_HPP
#define ARCH_ECS_TYPES_HPP

#include <cstdint>

namespace GLVM::ecs::arch {
#define ENTITY_ID_BITS 32
#define GENERATION_BITS 32

	constexpr uint64_t entityBitsMask = (1ull << ENTITY_ID_BITS) - 1;
	
	typedef uint64_t entity;
	typedef uint32_t id;
	typedef uint32_t generation;
	typedef uint64_t componentMask;

	struct ComponentsIndices {
		enum Types : uint32_t {
			TRANSFORM_COMPONENT,
			RIGID_BODY_COMPONENT,
			MESH_COMPONENT,
			FONT_COMPONENT,
			COLLIDER_COMPONENT,
			COLLIDER_FLAGS_COMPONENT,
			MATERIAL_COMPONENT,
			VIEW_COMPONENT,
			HEALTH_COMPONENT,
			ANIMATION_COMPONENT,
			STATE_COMPONENT,
			ENEMY_COMPONENT,
			DAMAGE_COMPONENT,
			ATTACK_COMPONENT,
			INVENTORY_COMPONENT,
			DIRECTIONAL_LIGHT_COMPONENT,
			SPOT_LIGHT_COMPONENT,
			POINT_LIGHT_COMPONENT,
			ITEM_COMPONENT,
			MOVE_COMPONENT,
			PROJECTILE_BUNDLE_COMPONENT,

			LEVEL_CHUNK_TAG_COMPONENT,
			PLAYER_TAG_COMPONENT,
			CROSSHAIR_TAG_COMPONENT,
			
			COMPONENTS_COUNT
		};
	};

	constexpr componentMask playerComponentMask =
		(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
		(1ull << ComponentsIndices::VIEW_COMPONENT) |
		(1ull << ComponentsIndices::COLLIDER_COMPONENT) |
		(1ull << ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
		(1ull << ComponentsIndices::MESH_COMPONENT) |
		(1ull << ComponentsIndices::RIGID_BODY_COMPONENT) |
		(1ull << ComponentsIndices::HEALTH_COMPONENT) |
		(1ull << ComponentsIndices::MATERIAL_COMPONENT) |
		(1ull << ComponentsIndices::MOVE_COMPONENT) |
		(1ull << ComponentsIndices::ATTACK_COMPONENT) |
		(1ull << ComponentsIndices::PLAYER_TAG_COMPONENT);

	constexpr componentMask enemyComponentMask =
		(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
		(1ull << ComponentsIndices::ENEMY_COMPONENT) |
		(1ull << ComponentsIndices::STATE_COMPONENT) |
		(1ull << ComponentsIndices::FONT_COMPONENT) |
		(1ull << ComponentsIndices::ANIMATION_COMPONENT) |
		(1ull << ComponentsIndices::MATERIAL_COMPONENT) |
		(1ull << ComponentsIndices::MESH_COMPONENT) |
		(1ull << ComponentsIndices::COLLIDER_COMPONENT) |
		(1ull << ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
		(1ull << ComponentsIndices::HEALTH_COMPONENT) |
		(1ull << ComponentsIndices::RIGID_BODY_COMPONENT) |
		(1ull << ComponentsIndices::ATTACK_COMPONENT) |
		(1ull << ComponentsIndices::MOVE_COMPONENT);

	constexpr componentMask staticMeshComponentMask =
		(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
		(1ull << ComponentsIndices::COLLIDER_COMPONENT) |
		(1ull << ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
		(1ull << ComponentsIndices::MESH_COMPONENT) |
		(1ull << ComponentsIndices::MATERIAL_COMPONENT) |
		(1ull << ComponentsIndices::FONT_COMPONENT);

	constexpr componentMask crosshairComponentMask =
		(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
		(1ull << ComponentsIndices::MESH_COMPONENT) |
		(1ull << ComponentsIndices::MATERIAL_COMPONENT) |
		(1ull << ComponentsIndices::CROSSHAIR_TAG_COMPONENT);

	constexpr componentMask inventoryComponentMask =
		(1ull << ComponentsIndices::TRANSFORM_COMPONENT) |
		(1ull << ComponentsIndices::INVENTORY_COMPONENT) |
		(1ull << ComponentsIndices::MATERIAL_COMPONENT);
};


#endif
