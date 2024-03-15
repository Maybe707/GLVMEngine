#ifndef DAMAGE_SYSTEM
#define DAMAGE_SYSTEM


#include "ISystem.hpp"

namespace GLVM::ecs
{
	class DamageSystem : public ISystem
	{
	public:
		void Update() override;
	};
} // namespace GLVM::ecs

#endif
