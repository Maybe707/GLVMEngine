#ifndef DAMAGE
#define DAMAGE

namespace GLVM::ecs::components
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
