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

namespace GLVM::ecs
{
	class DamageSystem : public ISystem
	{
	public:
		void Update() override;
	};
} // namespace GLVM::ecs

#endif
