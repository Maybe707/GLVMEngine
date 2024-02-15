#ifndef MATERIAL_COMPONENT
#define MATERIAL_COMPONENT

#include "Texture.hpp"
#include "VertexMath.hpp"

namespace GLVM::ecs::components
{
	struct material
	{
		ecs::TextureHandle diffuseTextureID_;
		ecs::TextureHandle specularTextureID_;
        unsigned int vkInnerId_; ///< This field using to choose specific instance of texture image in Vulkan.
		vec3 ambient;
		float shininess;
	};
}

#endif
