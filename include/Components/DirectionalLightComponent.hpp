#ifndef DIRECTIONAL_LIGHT_COMPONENT
#define DIRECTIONAL_LIGHT_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ecs
{
	struct directionalLight
	{
		vec3 position;
		vec3 direction;
		
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	};
}

#endif
