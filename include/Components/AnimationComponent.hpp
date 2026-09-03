#ifndef ANIMATION_COMPONENT_HPP
#define ANIMATION_COMPONENT_HPP

#include <cstdint>
#include "Vector.hpp"

namespace GLVM::ecs::components
{
	struct animation
	{
		uint32_t currentAnimationFrame = 0;
		float frameAccumulator = 0.0f;
		bool isAnimatedOnFrame = true;
		GLVM::core::vector<mat4> jointMatrices;
	};
} // namespace GLVM::ecs::components

#endif
