#ifndef DAMAGE_COMPONENT
#define DAMAGE_COMPONENT

namespace GAME_MECHANICS::ecs::components
{
	struct damage
	{
		float maximumDamage;
		float minimumDamage;
		float criticalHitRate;
		float criticalModifier;
	};
}

#endif
