#ifndef RENDER_CONFIG
#define RENDER_CONFIG

#include "ShaderStructs.hpp"
#include "VkStructs.hpp"
#include <vulkan/vulkan_core.h>

namespace GLVM::core
{
	inline DescriptorSet descriptorSetsConfig[32];
	inline DescriptorBinding descriptorBindingsConfig[32];
	inline Pipeline pipelineConfigs[32];
	
	inline void VkConfigInitializer() {
		/// ======================= DESCRIPTOR SETS AND ITS BINDINGS ============================ ///

		/// ===================================================================================== ///
		
		descriptorSetsConfig[SHADOW_MAP_DIRECTIONAL_LIGHT].actualLinkedDescriptorBindingsNumber = 1;
		descriptorSetsConfig[SHADOW_MAP_DIRECTIONAL_LIGHT].hostDescriptorNumber                 = 4000;
		descriptorSetsConfig[SHADOW_MAP_DIRECTIONAL_LIGHT].isTexture                            = false;

		descriptorBindingsConfig[0].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[0].type                    = DescriptorsTypes::DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO;
		descriptorBindingsConfig[0].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[0].binding                 = 0;
		descriptorBindingsConfig[0].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[0].uboChunkSize            = sizeof(ShadowMapMatrixUBO);
		
		/// ===================================================================================== ///

		descriptorSetsConfig[SHADOW_MAP_SPOT_LIGHT].actualLinkedDescriptorBindingsNumber        = 1;
		descriptorSetsConfig[SHADOW_MAP_SPOT_LIGHT].hostDescriptorNumber                        = 8000;
		descriptorSetsConfig[SHADOW_MAP_SPOT_LIGHT].isTexture                                   = false;

		descriptorBindingsConfig[1].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[1].type                    = DescriptorsTypes::SPOT_LIGHT_SHADOW_MAP_MATRIX_UBO;
		descriptorBindingsConfig[1].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[1].binding                 = 0;
		descriptorBindingsConfig[1].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[1].uboChunkSize            = sizeof(ShadowMapMatrixUBO);
		
		/// ===================================================================================== ///
		
		descriptorSetsConfig[SHADOW_MAP_POINT_LIGHT].actualLinkedDescriptorBindingsNumber       = 1;
		descriptorSetsConfig[SHADOW_MAP_POINT_LIGHT].hostDescriptorNumber                       = 128000;
		descriptorSetsConfig[SHADOW_MAP_POINT_LIGHT].isTexture                                  = false;

		descriptorBindingsConfig[2].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[2].type                    = DescriptorsTypes::POINT_LIGHT_SHADOW_MAP_MATRIX_UBO;
		descriptorBindingsConfig[2].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[2].binding                 = 0;
		descriptorBindingsConfig[2].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[2].uboChunkSize            = sizeof(PointLightShadowMapMatrixUBO);
		
		/// ===================================================================================== ///
		
		descriptorSetsConfig[HUD].actualLinkedDescriptorBindingsNumber                          = 1;
		descriptorSetsConfig[HUD].hostDescriptorNumber                                          = 1000;
		descriptorSetsConfig[HUD].isTexture                                                     = false;

		descriptorBindingsConfig[3].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[3].type                    = DescriptorsTypes::HUD_UBO;
		descriptorBindingsConfig[3].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[3].binding                 = 0;
		descriptorBindingsConfig[3].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[3].uboChunkSize            = sizeof(HUD_UBO);
		
		/// ===================================================================================== ///
		
		descriptorSetsConfig[FONT_RENDER_UBO].actualLinkedDescriptorBindingsNumber              = 1;
		descriptorSetsConfig[FONT_RENDER_UBO].hostDescriptorNumber                              = 2048;
		descriptorSetsConfig[FONT_RENDER_UBO].isTexture                                         = false;

		descriptorBindingsConfig[4].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[4].type                    = DescriptorsTypes::FONT_UBO;
		descriptorBindingsConfig[4].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[4].binding                 = 0;
		descriptorBindingsConfig[4].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[4].uboChunkSize            = sizeof(FONT_UBO);
		
		/// ===================================================================================== ///
		
		descriptorSetsConfig[FONT_RENDER_SAMPLER].actualLinkedDescriptorBindingsNumber          = 1;
		descriptorSetsConfig[FONT_RENDER_SAMPLER].hostDescriptorNumber                          = 18;
		descriptorSetsConfig[FONT_RENDER_SAMPLER].isTexture                                     = true;

		descriptorBindingsConfig[5].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[5].type                    = DescriptorsTypes::FONT_ATLAS_SAMPLER;
		descriptorBindingsConfig[5].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[5].binding                 = 0;
		descriptorBindingsConfig[5].shaderDescriptorsNumber = 1;
		
		/// ===================================================================================== ///
		
		descriptorSetsConfig[HUD_SCREEN].actualLinkedDescriptorBindingsNumber                   = 1;
		descriptorSetsConfig[HUD_SCREEN].hostDescriptorNumber                                   = 64;
		descriptorSetsConfig[HUD_SCREEN].isTexture                                              = false;

		descriptorBindingsConfig[6].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[6].type                    = DescriptorsTypes::HUD_SCREEN_UBO;
		descriptorBindingsConfig[6].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[6].binding                 = 0;
		descriptorBindingsConfig[6].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[6].uboChunkSize            = sizeof(HUD_SCREEN_UBO);
		
		/// ===================================================================================== ///
		
		descriptorSetsConfig[UI].actualLinkedDescriptorBindingsNumber                           = 1;
		descriptorSetsConfig[UI].hostDescriptorNumber                                           = 128;
		descriptorSetsConfig[UI].isTexture                                                      = false;

		descriptorBindingsConfig[7].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[7].type                    = DescriptorsTypes::UI_UBO;
		descriptorBindingsConfig[7].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[7].binding                 = 0;
		descriptorBindingsConfig[7].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[7].uboChunkSize            = sizeof(UI_UBO);
		
		/// ===================================================================================== ///

		descriptorSetsConfig[UI_SAMPLERS].actualLinkedDescriptorBindingsNumber                  = 1;
		descriptorSetsConfig[UI_SAMPLERS].hostDescriptorNumber                                  = 18;
		descriptorSetsConfig[UI_SAMPLERS].isTexture                                             = true;

		descriptorBindingsConfig[8].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[8].type                    = DescriptorsTypes::UI_SAMPLER;
		descriptorBindingsConfig[8].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[8].binding                 = 0;
		descriptorBindingsConfig[8].shaderDescriptorsNumber = 1;
		
		/// ===================================================================================== ///
		
		descriptorSetsConfig[UI_ICONS].actualLinkedDescriptorBindingsNumber                     = 1;
		descriptorSetsConfig[UI_ICONS].hostDescriptorNumber                                     = 128;
		descriptorSetsConfig[UI_ICONS].isTexture                                                = false;

		descriptorBindingsConfig[9].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[9].type                    = DescriptorsTypes::UI_ICONS_UBO;
		descriptorBindingsConfig[9].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[9].binding                 = 0;
		descriptorBindingsConfig[9].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[9].uboChunkSize            = sizeof(UI_UBO);
		
		/// ===================================================================================== ///

		descriptorSetsConfig[UI_ICONS_SAMPLERS].actualLinkedDescriptorBindingsNumber            = 1;
		descriptorSetsConfig[UI_ICONS_SAMPLERS].hostDescriptorNumber                            = 18;
		descriptorSetsConfig[UI_ICONS_SAMPLERS].isTexture                                       = true;

		descriptorBindingsConfig[10].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[10].type                    = DescriptorsTypes::UI_ICONS_SAMPLER;
		descriptorBindingsConfig[10].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[10].binding                 = 0;
		descriptorBindingsConfig[10].shaderDescriptorsNumber = 1;
		
		/// ===================================================================================== ///
		
		descriptorSetsConfig[VIRTUAL_TEXTURES_UBO].actualLinkedDescriptorBindingsNumber         = 1;
		descriptorSetsConfig[VIRTUAL_TEXTURES_UBO].hostDescriptorNumber                         = 128;
		descriptorSetsConfig[VIRTUAL_TEXTURES_UBO].isTexture                                    = false;

		descriptorBindingsConfig[11].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[11].type                    = DescriptorsTypes::VIRTUAL_TEXTURES_UBO;
		descriptorBindingsConfig[11].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[11].binding                 = 0;
		descriptorBindingsConfig[11].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[11].uboChunkSize            = sizeof(VIRTUAL_TEXTURE_UBO);
		
		/// ===================================================================================== ///

		descriptorSetsConfig[VIRTUAL_TEXTURES_TILESET].actualLinkedDescriptorBindingsNumber     = 1;
		descriptorSetsConfig[VIRTUAL_TEXTURES_TILESET].hostDescriptorNumber                     = 18;
		descriptorSetsConfig[VIRTUAL_TEXTURES_TILESET].isTexture                                = true;

		descriptorBindingsConfig[12].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[12].type                    = DescriptorsTypes::VIRTUAL_TEXTURE_TILESET;
		descriptorBindingsConfig[12].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[12].binding                 = 0;
		descriptorBindingsConfig[12].shaderDescriptorsNumber = 1;
		
		/// ===================================================================================== ///

		descriptorSetsConfig[MAIN_RENDER_MATRIX_UBO].actualLinkedDescriptorBindingsNumber       = 1;
		descriptorSetsConfig[MAIN_RENDER_MATRIX_UBO].hostDescriptorNumber                       = 1000;
		descriptorSetsConfig[MAIN_RENDER_MATRIX_UBO].isTexture                                  = false;

		descriptorBindingsConfig[13].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[13].type                    = DescriptorsTypes::MODEL_MATRIX_UBO;
		descriptorBindingsConfig[13].shaderStageFlag         = VK_SHADER_STAGE_VERTEX_BIT;
		descriptorBindingsConfig[13].binding                 = 0;
		descriptorBindingsConfig[13].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[13].uboChunkSize            = sizeof(ModelMatrixUBO);
		
		/// ===================================================================================== ///

		descriptorSetsConfig[MAIN_RENDER_LIGHT_DATA_UBO].actualLinkedDescriptorBindingsNumber   = 4;
		descriptorSetsConfig[MAIN_RENDER_LIGHT_DATA_UBO].hostDescriptorNumber                   = 2;
		descriptorSetsConfig[MAIN_RENDER_LIGHT_DATA_UBO].isTexture                              = false;

		descriptorBindingsConfig[14].vkType                  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorBindingsConfig[14].type                    = DescriptorsTypes::LIGHT_DATA;
		descriptorBindingsConfig[14].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[14].binding                 = 0;
		descriptorBindingsConfig[14].shaderDescriptorsNumber = 1;
		descriptorBindingsConfig[14].uboChunkSize            = sizeof(LightData);

		descriptorBindingsConfig[15].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[15].type                    = DescriptorsTypes::LIGHT_DATA;
		descriptorBindingsConfig[15].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[15].binding                 = 1;
		descriptorBindingsConfig[15].shaderDescriptorsNumber = 4;

		descriptorBindingsConfig[16].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[16].type                    = DescriptorsTypes::LIGHT_DATA;
		descriptorBindingsConfig[16].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[16].binding                 = 5;
		descriptorBindingsConfig[16].shaderDescriptorsNumber = 32;

		descriptorBindingsConfig[17].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[17].type                    = DescriptorsTypes::LIGHT_DATA;
		descriptorBindingsConfig[17].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[17].binding                 = 37;
		descriptorBindingsConfig[17].shaderDescriptorsNumber = 8;

		/// ===================================================================================== ///

		descriptorSetsConfig[MAIN_RENDER_SPECULAR_SAMPLER].actualLinkedDescriptorBindingsNumber = 1;
		descriptorSetsConfig[MAIN_RENDER_SPECULAR_SAMPLER].hostDescriptorNumber                 = 18;
		descriptorSetsConfig[MAIN_RENDER_SPECULAR_SAMPLER].isTexture                            = true;

		descriptorBindingsConfig[18].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[18].type                    = DescriptorsTypes::SPECULAR_SAMPLER;
		descriptorBindingsConfig[18].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[18].binding                 = 0;
		descriptorBindingsConfig[18].shaderDescriptorsNumber = 1;

		/// ===================================================================================== ///
		
		descriptorSetsConfig[MAIN_RENDER_DIFFUSE_SAMPLER].actualLinkedDescriptorBindingsNumber  = 1;
		descriptorSetsConfig[MAIN_RENDER_DIFFUSE_SAMPLER].hostDescriptorNumber                  = 18;
		descriptorSetsConfig[MAIN_RENDER_DIFFUSE_SAMPLER].isTexture                             = true;

		descriptorBindingsConfig[19].vkType                  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorBindingsConfig[19].type                    = DescriptorsTypes::DIFFUSE_SAMPLER;
		descriptorBindingsConfig[19].shaderStageFlag         = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorBindingsConfig[19].binding                 = 0;
		descriptorBindingsConfig[19].shaderDescriptorsNumber = 1;
		
		/// ===================================================================================== ///

		/// ======================== PIPELINES AND ITS RENDER PASSES ============================ ///

		/// ===================================================================================== ///

		pipelineConfigs[DIRECTIONAL_LIGHT_PIPELINE].vertShader                       = "../VKshaders/flatShadowMapShaders/vertFlatShadowMap.spv";
		pipelineConfigs[DIRECTIONAL_LIGHT_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[DIRECTIONAL_LIGHT_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[DIRECTIONAL_LIGHT_PIPELINE].actualLinkedDescriptorSetsNumber = 1;

		/// ===================================================================================== ///

		pipelineConfigs[SPOT_LIGHT_PIPELINE].vertShader                       = "../VKshaders/flatShadowMapShaders/vertFlatShadowMap.spv";
		pipelineConfigs[SPOT_LIGHT_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[SPOT_LIGHT_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[SPOT_LIGHT_PIPELINE].actualLinkedDescriptorSetsNumber = 1;
		
		/// ===================================================================================== ///

		pipelineConfigs[POINT_LIGHT_PIPELINE].vertShader                       = "../VKshaders/cubeShadowMapShaders/vertCubeShadowMap.spv";
		pipelineConfigs[POINT_LIGHT_PIPELINE].fragShader                       = "../VKshaders/cubeShadowMapShaders/fragCubeShadowMap.spv";
		pipelineConfigs[POINT_LIGHT_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[POINT_LIGHT_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[POINT_LIGHT_PIPELINE].actualLinkedDescriptorSetsNumber = 1;
		
		/// ===================================================================================== ///

		pipelineConfigs[HUD_PIPELINE].vertShader                       = "../VKshaders/hudShaders/hud_vert.spv";
		pipelineConfigs[HUD_PIPELINE].fragShader                       = "../VKshaders/hudShaders/hud_frag.spv";
		pipelineConfigs[HUD_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[HUD_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[HUD_PIPELINE].actualLinkedDescriptorSetsNumber = 1;
		
		/// ===================================================================================== ///

		pipelineConfigs[FONT_PIPELINE].vertShader                       = "../VKshaders/fontShaders/font_vert.spv";
		pipelineConfigs[FONT_PIPELINE].fragShader                       = "../VKshaders/fontShaders/font_frag.spv";
		pipelineConfigs[FONT_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[FONT_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[FONT_PIPELINE].actualLinkedDescriptorSetsNumber = 2;
		
		/// ===================================================================================== ///

		pipelineConfigs[HUD_SCREEN_PIPELINE].vertShader                       = "../VKshaders/hud_screen_shaders/vert_hud_screen.spv";
		pipelineConfigs[HUD_SCREEN_PIPELINE].fragShader                       = "../VKshaders/hud_screen_shaders/frag_hud_screen.spv";
		pipelineConfigs[HUD_SCREEN_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[HUD_SCREEN_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[HUD_SCREEN_PIPELINE].actualLinkedDescriptorSetsNumber = 1;
		
		/// ===================================================================================== ///

		pipelineConfigs[UI_PIPELINE].vertShader                       = "../VKshaders/ui_shaders/vert_ui.spv";
		pipelineConfigs[UI_PIPELINE].fragShader                       = "../VKshaders/ui_shaders/frag_ui.spv";
		pipelineConfigs[UI_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[UI_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[UI_PIPELINE].actualLinkedDescriptorSetsNumber = 2;
		
		/// ===================================================================================== ///

		pipelineConfigs[UI_ICONS_PIPELINE].vertShader                       = "../VKshaders/ui_icons_shaders/vert_ui_icons.spv";
		pipelineConfigs[UI_ICONS_PIPELINE].fragShader                       = "../VKshaders/ui_icons_shaders/frag_ui_icons.spv";
		pipelineConfigs[UI_ICONS_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[UI_ICONS_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[UI_ICONS_PIPELINE].actualLinkedDescriptorSetsNumber = 2;
		
		/// ===================================================================================== ///

		pipelineConfigs[VIRTUAL_TEXTURES_PIPELINE].vertShader                       = "../VKshaders/virtualTextures/virtualTexturesVert.spv";
		pipelineConfigs[VIRTUAL_TEXTURES_PIPELINE].fragShader                       = "../VKshaders/virtualTextures/virtualTexturesFrag.spv";
		pipelineConfigs[VIRTUAL_TEXTURES_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[VIRTUAL_TEXTURES_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[VIRTUAL_TEXTURES_PIPELINE].actualLinkedDescriptorSetsNumber = 2;
		
		/// ===================================================================================== ///

		pipelineConfigs[MAIN_RENDER_PIPELINE].vertShader                       = "../VKshaders/mainRendererShaders/vert.spv";
		pipelineConfigs[MAIN_RENDER_PIPELINE].fragShader                       = "../VKshaders/mainRendererShaders/frag.spv";
		pipelineConfigs[MAIN_RENDER_PIPELINE].bindingDescription               = Vertex::getBindingDescription();
		pipelineConfigs[MAIN_RENDER_PIPELINE].attributeDescriptions            = Vertex::getAttributeDescriptions();
		pipelineConfigs[MAIN_RENDER_PIPELINE].actualLinkedDescriptorSetsNumber = 4;
		
		/// ===================================================================================== ///

	}
	


	inline RenderPass renderPassConfigs[] = {
		{                                                                   ///< Directional light renderPass
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_1,
			{
				{ 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_1,
			{
				{ 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_1,
			{
				{ 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_1,
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
			ATTACHMENT_DESCRIPTION_NEXT_ARRAY_COUNT_2,
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
			ATTACHMENT_REFERENCE_NEXT_ARRAY_COUNT_2,
			{
				{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
				{ 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			},
			SUBPASS_DEPENDENCY_NEXT_ARRAY_COUNT_1,
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
