#ifndef RENDER_CONFIG
#define RENDER_CONFIG

#include "VkStructs.hpp"

namespace GLVM::core
{
	// inline DescriptorSet directionalLightDescriptorSet0{ {}, {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO,
	// 			VK_SHADER_STAGE_VERTEX_BIT, 0, 1}}, 512};
	// inline DescriptorBinding directionalLightDescriptorBinding0_Set0{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO,
	// 	VK_SHADER_STAGE_VERTEX_BIT, 0, 1 };
}; // namespace GLVM::core


#endif
