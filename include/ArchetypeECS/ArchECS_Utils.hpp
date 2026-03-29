#ifndef ARCH_ECS_UTILS_HPP
#define ARCH_ECS_UTILS_HPP

#include "ArchetypeECS/ArchECS_Types.hpp"
#include "ComponentsFullSet.hpp"
#include "TagComponents/LevelChunkTagComponent.hpp"
#include "TagComponents/PlayerTagComponent.hpp"

namespace GLVM::ecs::arch {
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

	entity     makeEntity( id id_, generation generation_ );
	id         getId( entity entity_ );
	generation getGen( entity entity_ );
	bool matchesRequiredMask(const componentMask archetypeMask, const componentMask& systemMask);
};

#endif
