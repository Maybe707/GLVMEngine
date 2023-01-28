#ifndef POINT_LIGHT_COMPONENT
#define POINT_LIGHT_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ecs
{
	struct pointLight
	{
		vec3 position;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;

		float constant;
		float linear;
		float quadratic;
	};
}

#endif
