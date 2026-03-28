#ifndef ANIMATION_COMPONENT
#define ANIMATION_COMPONENT

#include <cstdint>
namespace GLVM::ecs::components
{
	struct animation
	{
		uint32_t currentAnimationFrame = 0;
		float frameAccumulator = 0.0f;
	};
} // namespace GLVM::ecs::components

#endif
