// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef MATERIAL_COMPONENT
#define MATERIAL_COMPONENT

#include "Texture.hpp"
#include "VertexMath.hpp"

namespace GLVM::ecs::components
{
	struct material
	{
		ecs::TextureHandle diffuseTextureID_ = {};
		ecs::TextureHandle specularTextureID_ = {};
        unsigned int vkInnerId_ = 0; ///< This field using to choose specific instance of texture image in Vulkan.
		vec3 ambient = { 0.0f, 0.0f, 0.0f };
		float shininess = 0.0f;
	};
}

#endif
