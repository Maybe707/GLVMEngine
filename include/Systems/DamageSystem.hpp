// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef DAMAGE_SYSTEM
#define DAMAGE_SYSTEM


#include "ISystem.hpp"
#include "EntityManager.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs
{
	class DamageSystem : public ISystem
	{
	public:
		void Update() override;

		float deltaTime;
		arch::Archetype* cachedArchetypes[32];
		uint32_t cachedArchetypesNumber = 0;
		arch::Archetype* cachedFontArchetypes[32];
		uint32_t cachedFontArchetypesNumber = 0;
	};
} // namespace GLVM::ecs

#endif
