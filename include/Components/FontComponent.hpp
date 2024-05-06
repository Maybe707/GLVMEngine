#ifndef FONT_COMPONENT
#define FONT_COMPONENT

#include "Vector.hpp"

namespace GLVM::ecs::components
{
	struct font {
		core::vector<char> font_string;
		float lifeTime;
	};
} // namespace GLVM::ecs::components

#endif
