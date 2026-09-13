// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef DAMAGE_SYSTEM
#define DAMAGE_SYSTEM


#include "Systems/WorldSystem.hpp"
#include "EntityManager.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs
{
	class DamageSystem : public WorldSystem
	{
	public:
        explicit DamageSystem(arch::World& world) : WorldSystem(world) {}
		void Update() override;

		float deltaTime = {};

		uint32_t cachedAttackableArchetypesNumber = 0;
		uint32_t cachedFontArchetypesNumber       = 0;
		struct ArchView {
			core::vector<ecs::arch::Archetype*> cachedAttackableArchetypes;
			core::vector<ecs::arch::Archetype*> cachedFontArchetypes;
		} archView;
		
		struct ComponentsView {
			ecs::components::attack* attackableAttacks = nullptr;
			ecs::components::health* attackableHealth  = nullptr;
			ecs::components::font*   attackableFonts   = nullptr;

			ecs::components::font*   fonts = nullptr;
		} componentsView;

		arch::componentMask attackableRequiredMask =
			(1ul << arch::ComponentsIndices::ATTACK_COMPONENT) |
			(1ul << arch::ComponentsIndices::HEALTH_COMPONENT);

		arch::componentMask fontRequiredMask =
			(1ull << ecs::arch::ComponentsIndices::FONT_COMPONENT);
	};
} // namespace GLVM::ecs

#endif
