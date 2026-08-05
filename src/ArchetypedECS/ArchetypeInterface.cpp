#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/FontComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/ProjectileBundle.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "TagComponents/LevelChunkTagComponent.hpp"
#include "TagComponents/PlayerTagComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "TagComponents/ProjectileTagComponent.hpp"

namespace GLVM::ecs::arch {
	uint32_t Archetype::addEntity( entity entity_ ) {
		uint32_t index = entityCount++;
		assert( index < CAPACITY );
		entities[index] = entity_;
		
		return index;
	}
	
	/// Swap-remove
	entity Archetype::removeEntity( uint32_t index ) {
		uint32_t last = entityCount - 1;

		for( uint32_t i = 0; i < componentCount; ++i ) {
			const uint32_t componentId = componentIds[i];

			switch( componentId ) {
			case ComponentsIndices::TRANSFORM_COMPONENT:
				static_cast<components::transform*>(components[componentId])[index] = static_cast<components::transform*>(components[componentId])[last];
				break;
			case ComponentsIndices::RIGID_BODY_COMPONENT:
				static_cast<components::rigidBody*>(components[componentId])[index] = static_cast<components::rigidBody*>(components[componentId])[last];
				break;
			case ComponentsIndices::MESH_COMPONENT:
				static_cast<components::mesh*>(components[componentId])[index] = static_cast<components::mesh*>(components[componentId])[last];
				break;
			case ComponentsIndices::FONT_COMPONENT:
				static_cast<components::font*>(components[componentId])[index] = static_cast<components::font*>(components[componentId])[last];
				break;
			case ComponentsIndices::COLLIDER_COMPONENT:
				static_cast<components::collider*>(components[componentId])[index] = static_cast<components::collider*>(components[componentId])[last];
				break;
			case ComponentsIndices::COLLIDER_FLAGS_COMPONENT:
				static_cast<components::colliderFlags*>(components[componentId])[index] = static_cast<components::colliderFlags*>(components[componentId])[last];
				break;
			case ComponentsIndices::MATERIAL_COMPONENT:
				static_cast<components::material*>(components[componentId])[index] = static_cast<components::material*>(components[componentId])[last];
				break;
			case ComponentsIndices::VIEW_COMPONENT:
				static_cast<components::beholder*>(components[componentId])[index] = static_cast<components::beholder*>(components[componentId])[last];
				break;
			case ComponentsIndices::HEALTH_COMPONENT:
				static_cast<components::health*>(components[componentId])[index] = static_cast<components::health*>(components[componentId])[last];
				break;
			case ComponentsIndices::ANIMATION_COMPONENT:
				static_cast<components::animation*>(components[componentId])[index] = static_cast<components::animation*>(components[componentId])[last];
				break;
			case ComponentsIndices::STATE_COMPONENT:
				static_cast<components::state*>(components[componentId])[index] = static_cast<components::state*>(components[componentId])[last];
				break;
			case ComponentsIndices::ENEMY_COMPONENT:
				static_cast<components::enemy*>(components[componentId])[index] = static_cast<components::enemy*>(components[componentId])[last];
				break;
			case ComponentsIndices::DAMAGE_COMPONENT:
				static_cast<components::damage*>(components[componentId])[index] = static_cast<components::damage*>(components[componentId])[last];
				break;
			case ComponentsIndices::ATTACK_COMPONENT:
				static_cast<components::attack*>(components[componentId])[index] = static_cast<components::attack*>(components[componentId])[last];
				break;
			case ComponentsIndices::INVENTORY_COMPONENT:
				static_cast<components::inventory*>(components[componentId])[index] = static_cast<components::inventory*>(components[componentId])[last];
				break;
			case ComponentsIndices::DIRECTIONAL_LIGHT_COMPONENT:
				static_cast<components::directionalLight*>(components[componentId])[index] = static_cast<components::directionalLight*>(components[componentId])[last];
				break;
			case ComponentsIndices::SPOT_LIGHT_COMPONENT:
				static_cast<components::spotLight*>(components[componentId])[index] = static_cast<components::spotLight*>(components[componentId])[last];
				break;
			case ComponentsIndices::POINT_LIGHT_COMPONENT:
				static_cast<components::pointLight*>(components[componentId])[index] = static_cast<components::pointLight*>(components[componentId])[last];
				break;
			case ComponentsIndices::ITEM_COMPONENT:
				static_cast<components::item*>(components[componentId])[index] = static_cast<components::item*>(components[componentId])[last];
				break;
			case ComponentsIndices::MOVE_COMPONENT:
				static_cast<components::move*>(components[componentId])[index] = static_cast<components::move*>(components[componentId])[last];
				break;
			case ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT:
				static_cast<ProjectileBundle*>(components[componentId])[index] = static_cast<ProjectileBundle*>(components[componentId])[last];
				break;
			case ComponentsIndices::LEVEL_CHUNK_TAG_COMPONENT:
				static_cast<tagComponents::levelChunkTagComponent*>(components[componentId])[index] = static_cast<tagComponents::levelChunkTagComponent*>(components[componentId])[last];
			case ComponentsIndices::PROJECTILE_TAG_COMPONENT:
				static_cast<tagComponents::projectileTagComponent*>(components[componentId])[index] = static_cast<tagComponents::projectileTagComponent*>(components[componentId])[last];
				break;
			case ComponentsIndices::PLAYER_TAG_COMPONENT:
				static_cast<tagComponents::playerTagComponent*>(components[componentId])[index] = static_cast<tagComponents::playerTagComponent*>(components[componentId])[last];
				break;
			}
		}
		
		entity moved = entities[last];
		entities[index] = moved;
		--entityCount;

		return moved;
	}
}; // namespace GLVM::ecs::arch
