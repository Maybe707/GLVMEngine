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
