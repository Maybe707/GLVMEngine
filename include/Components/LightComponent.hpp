#ifndef LIGHT_COMPONENT
#define LIGHT_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::Core
{
	struct SLightComponent
	{
		vec3 position;
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	};
}

#endif
