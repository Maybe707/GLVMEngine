#ifndef VK_STRUCTS
#define VK_STRUCTS

#include <math.h>
#include "ShaderStructs.hpp"
#include "WavefrontObjParser.hpp"
#include <vulkan/vulkan_core.h>

namespace GLVM::core
{
	struct MeshAxisMaxAbsoluteValues {
		float absolute_x = 0.0f;
		float absolute_y = 0.0f;
		float absolute_z = 0.0f;
	};

	struct MeshAxisLimitingValues {
		float lowest_x = MAXFLOAT;
		float highest_x = -MAXFLOAT;
		float lowest_y = MAXFLOAT;
		float highest_y = -MAXFLOAT;
		float lowest_z = MAXFLOAT;
		float highest_z = -MAXFLOAT;

		void setToDefaultValues() {
			highest_x = -MAXFLOAT;
			lowest_x  = MAXFLOAT;
			highest_y = -MAXFLOAT;
			lowest_y  = MAXFLOAT;
			highest_z = -MAXFLOAT;
			lowest_z  = MAXFLOAT;
		}

		void comparePerDirectionAndSetToMaximumValueByModule( SVertex& vertex ) {
			if ( vertex[0] < lowest_x ) {
				lowest_x = vertex[0];
			} else if ( vertex[0] > highest_x ) {
				highest_x = vertex[0];
			}

			if ( vertex[1] < lowest_y ) {
				lowest_y = vertex[1];
			} else if ( vertex[1] > highest_y ) {
				highest_y = vertex[1];
			}

			if ( vertex[2] < lowest_z ) {
				lowest_z = vertex[2];
			} else if ( vertex[2] > highest_z ) {
				highest_z = vertex[2];
			}
		}

		void comparePerDirectionAndSetToMaximumValueByModule( vec3 position, float half_x, float half_y, float half_z ) {
			if ( position[0] + half_x > highest_x ) {
				highest_x = position[0] + half_x;
			}
			if ( position[0] - half_x < lowest_x ) {
				lowest_x = position[0] - half_x;
			}
			if ( position[1] + half_y > highest_y ) {
				highest_y = position[1] + half_y;
			}
			if ( position[1] - half_y < lowest_y ) {
				lowest_y = position[1] - half_y;
			}
			if ( position[2] + half_z > highest_z ) {
				highest_z = position[2] + half_z;
			}
			if ( position[2] - half_z < lowest_z ) {
				lowest_z = position[2] - half_z;
			}
		}
	};
	
	enum class DescriptorsTypes {
		/// UBO - uniform buffer object
		DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO,
		SPOT_LIGHT_SHADOW_MAP_MATRIX_UBO,
		POINT_LIGHT_SHADOW_MAP_MATRIX_UBO,
		HUD_UBO,
		FONT_UBO,
		FONT_ATLAS_SAMPLER,
		HUD_SCREEN_UBO,
		UI_UBO,
		UI_SAMPLER,
		UI_ICONS_UBO,
		UI_ICONS_SAMPLER,
		VIRTUAL_TEXTURES_UBO,
		VIRTUAL_TEXTURE_TILESET,
		MODEL_MATRIX_UBO,

		LIGHT_DATA,
		SPECULAR_SAMPLER,
		DIFFUSE_SAMPLER,
	};

	enum DescriptorSetDataLink {
		SHADOW_MAP_DIRECTIONAL_LIGHT,
		SHADOW_MAP_SPOT_LIGHT,
		SHADOW_MAP_POINT_LIGHT,
		HUD,
		FONT_RENDER_UBO,
		FONT_RENDER_SAMPLER,
		HUD_SCREEN,
		UI,
		UI_SAMPLERS,
		UI_ICONS,
		UI_ICONS_SAMPLERS,
		VIRTUAL_TEXTURES_UBO,
		VIRTUAL_TEXTURES_TILESET,
		MAIN_RENDER_MATRIX_UBO,
		MAIN_RENDER_LIGHT_DATA_UBO,
		MAIN_RENDER_SPECULAR_SAMPLER,
		MAIN_RENDER_DIFFUSE_SAMPLER,
		DESCRIPTOR_CHUNKS_NUMBER
	};
	
	enum SpecificPipeline {
		DIRECTIONAL_LIGHT_PIPELINE,
		SPOT_LIGHT_PIPELINE,
		POINT_LIGHT_PIPELINE,
		HUD_PIPELINE,
		FONT_PIPELINE,
		HUD_SCREEN_PIPELINE,
		UI_PIPELINE,
		UI_ICONS_PIPELINE,
		VIRTUAL_TEXTURES_PIPELINE,
		MAIN_RENDER_PIPELINE,
		PIPELINES_NUMBER
	};
	
	struct VK_Image {
		VkImage image = {};
		VkDeviceMemory deviceMemory = {};
		std::vector<VkImageView> views = {};
		VkImageViewType viewType = {};
		VkImageCreateFlags createFlags = {};
		VkMemoryPropertyFlags memoryPropertyFlags = {};
		VkImageUsageFlags usageFlags = {};
		VkImageAspectFlags aspectFlags = {};
		VkFormat format = {};
		VkImageTiling tiling = {};
		VkSampler sampler = {};
		VkComponentSwizzle red = {};
		VkComponentSwizzle green = {};
		VkComponentSwizzle blue = {};
		VkComponentSwizzle alpha = {};
		uint32_t arrayLayers = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};
	
	struct DescriptorBinding {                 ///< Meta data for descriptor bindings
		VkDescriptorType       vkType;
		DescriptorsTypes       type;
		VkShaderStageFlags     shaderStageFlag;
		unsigned int           binding;
		unsigned int           shaderDescriptorsNumber;
		unsigned int           globalDescriptorOffset;
		VkDeviceSize           uboChunkSize;
	};

	struct DescriptorSet {                     ///< Meta data for descriptor sets
		unsigned int                    actualLinkedDescriptorBindingsNumber;
		unsigned int                    hostDescriptorNumber;
		VkDescriptorSetLayout           setLayout;
		static constexpr unsigned int   maximumLinkedDescriptorBindingsDS = 32;
		unsigned int                    descriptorsBindingsIDs[maximumLinkedDescriptorBindingsDS];
		unsigned int                    descriptorSetOffset;
		bool                   isTexture;
	};
	
	struct Pipeline {
		VkPipeline  pipeline;
		VkPipelineLayout pipelineLayout;
		const char* vertShader = nullptr;
		const char* fragShader = nullptr;
		VkVertexInputBindingDescription bindingDescription;
		std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions;
		unsigned int                    actualLinkedDescriptorSetsNumber;
		static constexpr unsigned int   maximumLinkedDescriptorSetDS = 32;
		unsigned int                    linkedDescriptorSetIDs[maximumLinkedDescriptorSetDS];
	};

	struct GPUBuffer {
		VkBuffer buffer;
		VkDeviceMemory deviceMemory;
		VkDeviceSize uboChunkSize;
	};

	union Descriptor {
		Descriptor() {};
		~Descriptor() {};
		
		GPUBuffer* GPUBuffer;
		VK_Image* GPUImage;
	};
	
	struct Vertex {
        vec3 pos;
        vec3 color;
        vec2 texCoord;
		vec4 joinIndices;
		vec4 weights;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescription.stride = sizeof(Vertex);

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, joinIndices);

            attributeDescriptions[4].binding = 0;
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[4].offset = offsetof(Vertex, weights);
			
            return attributeDescriptions;
        }
    };
} 

#endif
