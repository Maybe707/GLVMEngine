#ifndef DAMAGE_ARCHETYPE
#define DAMAGE_ARCHETYPE

#include "Components/AttackComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/FontComponent.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs::arch {
	constexpr uint32_t DAMAGE_ARCH_CHUNK_SIZE =
		ARCHETYPE_CHUNK_SIZE /
		(sizeof(components::attack) +
		 sizeof(components::health) +
		 sizeof(components::font));
	
	struct DamageArchetype : Archetype {
		components::attack attacks[DAMAGE_ARCH_CHUNK_SIZE];
		components::health health[DAMAGE_ARCH_CHUNK_SIZE];
		components::font   fonts[DAMAGE_ARCH_CHUNK_SIZE];

		DamageArchetype() {
            registerComponent<ComponentsIndices::ATTACK_COMPONENT>(attacks);
            registerComponent<ComponentsIndices::HEALTH_COMPONENT>(health);
            registerComponent<ComponentsIndices::FONT_COMPONENT>(fonts);
        }
        std::unique_ptr<Archetype> cloneEmpty() const override { return std::make_unique<DamageArchetype>(); }
	};
}; // namespace GLVM::ecs::arch

#endif
