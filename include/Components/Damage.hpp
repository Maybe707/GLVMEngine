#ifndef DAMAGE
#define DAMAGE

namespace GAME_MECHANICS::ecs::components
{
	struct damage
	{
		float maxDamage;
		float minDamage;
		float critRate;
		float critHit;
	};
}

#endif
