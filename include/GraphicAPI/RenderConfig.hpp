#ifndef RENDER_CONFIG
#define RENDER_CONFIG

#include "VkStructs.hpp"

namespace GLVM::core
{
	inline DescriptorSet     descriptorSets[] = {
		{1, 4000,   {}, {} },                     ///< Directional light
		{1, 8000,   {}, {} },                     ///< Spot light
		{1, 128000, {}, {} },                     ///< Point light
		{1, 1000,   {}, {} },                     ///< Hud
		{1, 2048,   {}, {} },                     ///< Font ubo
		{1, 2,      {}, {} },                     ///< Font sampler
		{1, 64,     {}, {} },                     ///< Hudscreen
		{1, 128,    {}, {} },                     ///< UI ubo
		{1, 2,      {}, {} },                     ///< UI sampler
		{1, 128,    {}, {} },                     ///< UIIcons ubo
		{1, 9,      {}, {} },                     ///< UIIcons sampler
		{1, 64,     {}, {} },                     ///< Virtual textures ubo
		{1, 2,      {}, {} },                     ///< Virtual textures sampler
		{1, 1000,   {}, {} },                     ///< Main render matrix ubo
		{4, 2,      {}, {} },                     ///< Main render light data
		{1, 18,     {}, {} },                     ///< Main render specular sampler
		{1, 18,     {}, {} },                     ///< Main render diffuse sampler
	};  
	inline DescriptorBinding descriptorBindings[] = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO, VK_SHADER_STAGE_VERTEX_BIT, 0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::SPOT_LIGHT_SHADOW_MAP_MATRIX_UBO, VK_SHADER_STAGE_VERTEX_BIT,        0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::POINT_LIGHT_SHADOW_MAP_MATRIX_UBO, VK_SHADER_STAGE_VERTEX_BIT,       0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::HUD_UBO, VK_SHADER_STAGE_VERTEX_BIT,                                 0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::FONT_UBO, VK_SHADER_STAGE_VERTEX_BIT,                                0, 1 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::FONT_ATLAS_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,            0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::HUD_SCREEN_UBO, VK_SHADER_STAGE_VERTEX_BIT,                          0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::UI_UBO, VK_SHADER_STAGE_VERTEX_BIT,                                  0, 1 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::UI_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,                    0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::UI_ICONS_UBO, VK_SHADER_STAGE_VERTEX_BIT,                            0, 1 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::UI_ICONS_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,              0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::VIRTUAL_TEXTURES_UBO, VK_SHADER_STAGE_VERTEX_BIT,                    0, 1 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::VIRTUAL_TEXTURE_TILESET, VK_SHADER_STAGE_FRAGMENT_BIT,       0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::MODEL_MATRIX_UBO, VK_SHADER_STAGE_VERTEX_BIT,                        0, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::LIGHT_DATA, VK_SHADER_STAGE_FRAGMENT_BIT,                            0, 1 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::LIGHT_DATA, VK_SHADER_STAGE_FRAGMENT_BIT,                    1, 4 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::LIGHT_DATA, VK_SHADER_STAGE_FRAGMENT_BIT,                    5, 32 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::LIGHT_DATA, VK_SHADER_STAGE_FRAGMENT_BIT,                    37, 8 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::SPECULAR_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,              0, 1 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::DIFFUSE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,               0, 1 }
	};
	// inline DescriptorSet directionalLightDescriptorSet0{ {}, {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO,
	// 			VK_SHADER_STAGE_VERTEX_BIT, 0, 1}}, 512};
	// inline DescriptorBinding directionalLightDescriptorBinding0_Set0{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO,
	// 	VK_SHADER_STAGE_VERTEX_BIT, 0, 1 };
}; // namespace GLVM::core


#endif
