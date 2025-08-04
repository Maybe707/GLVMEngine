#ifndef RENDER_CONFIG
#define RENDER_CONFIG

#include "VkStructs.hpp"
#include <vulkan/vulkan_core.h>

namespace GLVM::core
{
	inline const char* vertShaderMain_ = "../VKshaders/mainRendererShaders/vert.spv";
	inline const char* fragShaderMain_ = "../VKshaders/mainRendererShaders/frag.spv";

	inline const char* vertShaderFlatShadowMap = "../VKshaders/flatShadowMapShaders/vertFlatShadowMap.spv";
	inline const char* fragShaderDirectionalLightShadowMap = "../VKshaders/flatShadowMapShaders/fragFlatShadowMap.spv";

	inline const char* vertShaderCubeShadowMap = "../VKshaders/cubeShadowMapShaders/vertCubeShadowMap.spv";
	inline const char* fragShaderCubeShadowMap = "../VKshaders/cubeShadowMapShaders/fragCubeShadowMap.spv";

	inline const char* vertShaderHUD = "../VKshaders/hudShaders/hud_vert.spv";
	inline const char* fragShaderHUD = "../VKshaders/hudShaders/hud_frag.spv";

	inline const char* vertShaderFont = "../VKshaders/fontShaders/font_vert.spv";
	inline const char* fragShaderFont = "../VKshaders/fontShaders/font_frag.spv";

	inline const char* vertexShaderHudScreen = "../VKshaders/hud_screen_shaders/vert_hud_screen.spv";
	inline const char* fragmentShaderHudScreen = "../VKshaders/hud_screen_shaders/frag_hud_screen.spv";

	inline const char* vertexShaderUI = "../VKshaders/ui_shaders/vert_ui.spv";
	inline const char* fragmentShaderUI = "../VKshaders/ui_shaders/frag_ui.spv";

	inline const char* vertexShaderIconsUI = "../VKshaders/ui_icons_shaders/vert_ui_icons.spv";
	inline const char* fragmentShaderIconsUI = "../VKshaders/ui_icons_shaders/frag_ui_icons.spv";

	inline const char* virtualTexturesVertexShader = "../VKshaders/virtualTextures/virtualTexturesVert.spv";
	inline const char* virtualTexturesFragmentShader = "../VKshaders/virtualTextures/virtualTexturesFrag.spv";
	
	inline DescriptorSet     descriptorSetsConfig[] = {
		{1, 4000,   {}, {}, {}, false },                     ///< Directional light
		{1, 8000,   {}, {}, {}, false },                     ///< Spot light
		{1, 128000, {}, {}, {}, false },                     ///< Point light
		{1, 1000,   {}, {}, {}, false },                     ///< Hud
		{1, 2048,   {}, {}, {}, false },                     ///< Font ubo
		{1, 2,      {}, {}, {}, true },                     ///< Font sampler
		{1, 64,     {}, {}, {}, false },                     ///< Hudscreen
		{1, 128,    {}, {}, {}, false },                     ///< UI ubo
		{1, 2,      {}, {}, {}, true },                     ///< UI sampler
		{1, 128,    {}, {}, {}, false },                     ///< UIIcons ubo
		{1, 9,      {}, {}, {}, true },                     ///< UIIcons sampler
		{1, 128,     {}, {}, {}, false },                     ///< Virtual textures ubo
		{1, 2,      {}, {}, {}, true },                     ///< Virtual textures sampler
		{1, 1000,   {}, {}, {}, false },                     ///< Main render matrix ubo
		{4, 2,      {}, {}, {}, false },                     ///< Main render light data
		{1, 18,     {}, {}, {}, true },                     ///< Main render specular sampler
		{1, 18,     {}, {}, {}, true },                     ///< Main render diffuse sampler
	};
	
	inline DescriptorBinding descriptorBindingsConfig[] = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO, VK_SHADER_STAGE_VERTEX_BIT, 0, 1, {}, sizeof(ShadowMapMatrixUBO) },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::SPOT_LIGHT_SHADOW_MAP_MATRIX_UBO, VK_SHADER_STAGE_VERTEX_BIT,        0, 1, {}, sizeof(ShadowMapMatrixUBO) },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::POINT_LIGHT_SHADOW_MAP_MATRIX_UBO, VK_SHADER_STAGE_VERTEX_BIT,       0, 1, {}, sizeof(PointLightShadowMapMatrixUBO) },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::HUD_UBO, VK_SHADER_STAGE_VERTEX_BIT,                                 0, 1, {}, sizeof(HUD_UBO) },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::FONT_UBO, VK_SHADER_STAGE_VERTEX_BIT,                                0, 1, {}, sizeof(FONT_UBO) },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::FONT_ATLAS_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,            0, 1, {}, 0 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::HUD_SCREEN_UBO, VK_SHADER_STAGE_VERTEX_BIT,                          0, 1, {}, sizeof(HUD_SCREEN_UBO) },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::UI_UBO, VK_SHADER_STAGE_VERTEX_BIT,                                  0, 1, {}, sizeof(UI_UBO) },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::UI_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,                    0, 1, {}, 0 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::UI_ICONS_UBO, VK_SHADER_STAGE_VERTEX_BIT,                            0, 1, {}, sizeof(UI_UBO) },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::UI_ICONS_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,              0, 1, {}, 0 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::VIRTUAL_TEXTURES_UBO, VK_SHADER_STAGE_VERTEX_BIT,                    0, 1, {}, sizeof(VIRTUAL_TEXTURE_UBO) },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::VIRTUAL_TEXTURE_TILESET, VK_SHADER_STAGE_FRAGMENT_BIT,       0, 1, {}, 0 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::MODEL_MATRIX_UBO, VK_SHADER_STAGE_VERTEX_BIT,                        0, 1, {}, sizeof(ModelMatrixUBO) },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::LIGHT_DATA, VK_SHADER_STAGE_FRAGMENT_BIT,                            0, 1, {}, sizeof(LightData) },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::LIGHT_DATA, VK_SHADER_STAGE_FRAGMENT_BIT,                    1, 4, {}, 0 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::LIGHT_DATA, VK_SHADER_STAGE_FRAGMENT_BIT,                    5, 32, {}, 0 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::LIGHT_DATA, VK_SHADER_STAGE_FRAGMENT_BIT,                    37, 8, {}, 0 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::SPECULAR_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,              0, 1, {}, 0 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorsTypes::DIFFUSE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,               0, 1, {}, 0 }
	};

	inline Pipeline pipelineConfigs[] = {
		{ {}, {}, vertShaderFlatShadowMap, nullptr, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 1, {} },                            ///< directionalLightPipeline
		{ {}, {}, vertShaderFlatShadowMap, nullptr, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 1, {} },                            ///< spotLightPipeline
		{ {}, {}, vertShaderCubeShadowMap, fragShaderCubeShadowMap, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 1, {} },            ///< pointLightPipeline
		{ {}, {}, vertShaderHUD, fragShaderHUD, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 1, {} },                                ///< hudPipeline  
		{ {}, {}, vertShaderFont, fragShaderFont, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 2, {} },                              ///< fontPipeline
		{ {}, {}, vertexShaderHudScreen, fragmentShaderHudScreen, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 1, {} },              ///< hudScreenPipeline
		{ {}, {}, vertexShaderUI, fragmentShaderUI, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 2, {} },                            ///< uiPipeline
		{ {}, {}, vertexShaderIconsUI, fragmentShaderIconsUI, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 2, {} },                  ///< uiIconsPipeline
		{ {}, {}, virtualTexturesVertexShader, virtualTexturesFragmentShader, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 2, {} },  ///< virtualTexturesPipeline
		{ {}, {}, vertShaderMain_, fragShaderMain_, Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(), 4, {} },                            ///< mainRenderPipeline
	};

	inline RenderPass renderPassConfigs[] = {
		{                                                                   ///< Directional light renderPass
			1,
			{
				{
					0,
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
				}
			},
			1,
			{
				{ 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			2,
			{
				{ 
					VK_SUBPASS_EXTERNAL,
					0,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				},
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				}
			}
		},
		{                                                                   ///< Spot light renderPass
			1,
			{
				{
					0,
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
				}
			},
			1,
			{
				{ 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			2,
			{
				{ 
					VK_SUBPASS_EXTERNAL,
					0,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				},
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				}
			}
		}, 
		{                                                                   ///< Point light renderPass
			1,
			{
				{
					0,
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
				}
			},
			1,
			{
				{ 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			2,
			{
				{ 
					VK_SUBPASS_EXTERNAL,
					0,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				},
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				}
			}
		}, 
		{                                                                     ///< Hud renderPass
			2,
			{
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}
			},
			2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			1,
			{
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					{},
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					{}
				}
			}
		},
		{                                                                     ///< Font renderPass
			2,
			{
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}
			},
			2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			1,
			{
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					{},
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					{}
				}
			}
		},
		{                                                                     ///< Hud screen renderPass
			2,
			{
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}
			},
			2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			1,
			{
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					{},
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					{}
				}
			}
		},
		{                                                                     ///< UI renderPass
			2,
			{
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}
			},
			2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			1,
			{
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					{},
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					{}
				}
			}
		},
		{                                                                     ///< UIIcons renderPass
			2,
			{
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}
			},
			2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			1,
			{
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					{},
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					{}
				}
			}
		},
		{                                                                     ///< Virtual texture renderPass
			2,
			{
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}
			},
			2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			1,
			{
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					{},
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					{}
				}
			}
		},
		{                                                                     ///< Main render renderPass
			2,
			{
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
				{
					{},
					{},
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}
			},
			2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			1,
			{
				{
					0,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
					{},
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
					{}
				}
			}
		}
	};
	// inline DescriptorSet directionalLightDescriptorSet0{ {}, {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO,
	// 			VK_SHADER_STAGE_VERTEX_BIT, 0, 1}}, 512};
	// inline DescriptorBinding directionalLightDescriptorBinding0_Set0{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO,
	// 	VK_SHADER_STAGE_VERTEX_BIT, 0, 1 };
}; // namespace GLVM::core


#endif
