#ifndef PLAYER_ARCHETYPE_HPP
#define PLAYER_ARCHETYPE_HPP

#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "Components/FontComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "TagComponents/PlayerTagComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Components/RotationComponent.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t PLAYER_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::transform) +
		 sizeof(components::beholder) +
		 sizeof(components::collider) +
		 sizeof(components::colliderFlags) +
		 sizeof(components::mesh) +
		 sizeof(components::rigidBody) +
		 sizeof(components::health) +
		 sizeof(components::material) +
		 sizeof(components::move) +
		 sizeof(components::attack) +
		 sizeof(components::animation) +
		 sizeof(components::font) +
		 sizeof(components::rotation) +
		 sizeof(tagComponents::playerTagComponent));
	
	struct PlayerArchetype : Archetype {
		components::transform             transforms[PLAYER_ARCH_CHUNK_SIZE];
		components::beholder              beholders[PLAYER_ARCH_CHUNK_SIZE];
		components::collider              colliders[PLAYER_ARCH_CHUNK_SIZE];
		components::colliderFlags         colliderFlags[PLAYER_ARCH_CHUNK_SIZE];
		components::mesh                  meshes[PLAYER_ARCH_CHUNK_SIZE];
		components::rigidBody             rigidBodies[PLAYER_ARCH_CHUNK_SIZE];
		components::health                health[PLAYER_ARCH_CHUNK_SIZE];
		components::material              materials[PLAYER_ARCH_CHUNK_SIZE];
		components::move                  moves[PLAYER_ARCH_CHUNK_SIZE];
		components::attack                attacks[PLAYER_ARCH_CHUNK_SIZE];
		components::animation             animations[PLAYER_ARCH_CHUNK_SIZE];
		components::font                  fonts[PLAYER_ARCH_CHUNK_SIZE];
		components::rotation              rotations[PLAYER_ARCH_CHUNK_SIZE];
		tagComponents::playerTagComponent playerTagComponents[PLAYER_ARCH_CHUNK_SIZE];

		PlayerArchetype() {
            registerComponent<ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
            registerComponent<ComponentsIndices::VIEW_COMPONENT>(beholders);
            registerComponent<ComponentsIndices::COLLIDER_COMPONENT>(colliders);
            registerComponent<ComponentsIndices::COLLIDER_FLAGS_COMPONENT>(colliderFlags);
            registerComponent<ComponentsIndices::MESH_COMPONENT>(meshes);
            registerComponent<ComponentsIndices::RIGID_BODY_COMPONENT>(rigidBodies);
            registerComponent<ComponentsIndices::HEALTH_COMPONENT>(health);
            registerComponent<ComponentsIndices::MATERIAL_COMPONENT>(materials);
            registerComponent<ComponentsIndices::MOVE_COMPONENT>(moves);
            registerComponent<ComponentsIndices::ATTACK_COMPONENT>(attacks);
            registerComponent<ComponentsIndices::ANIMATION_COMPONENT>(animations);
            registerComponent<ComponentsIndices::FONT_COMPONENT>(fonts);
            registerComponent<ComponentsIndices::ROTATION_COMPONENT>(rotations);
            registerComponent<ComponentsIndices::PLAYER_TAG_COMPONENT>(playerTagComponents);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<PlayerArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
