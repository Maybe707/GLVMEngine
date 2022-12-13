#ifndef MATERIAL_COMPONENT
#define MATERIAL_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::Core
{
	struct SMaterialComponent
	{
		unsigned int id_;
        unsigned int vkInnerId_; ///< This field using to choose specific instance of texture image in Vulkan.
		vec3 ambient;
		// vec3 diffuse;
//		unsigned int diffuse;
		vec3 specular;
		float shininess;
	};
}

#endif
