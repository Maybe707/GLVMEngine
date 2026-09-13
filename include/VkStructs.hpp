#ifndef VK_STRUCTS
#define VK_STRUCTS

#include <math.h>
#include "Rendering/RenderTypes.hpp"
#include "WavefrontObjParser.hpp"
#include <vulkan/vulkan_core.h>
#include <array>

#ifdef _WIN32
#include <cfloat>
#define MAXFLOAT FLT_MAX
#endif

namespace GLVM::core
{
	enum DescriptorSetDataLink {
		/// Pipelines related values
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
		SDF_DATA,
		COLLISIONS_DEBUG_DATA,
		SPACIAL_GRID_DEBUG_DATA,
		/// Not related to any pipeline values
		RIDABLE_TEXTURES,
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
		SDF_PIPELINE,
		COLLISIONS_DEBUG_PIPELINE,
		SPACIAL_GRID_DEBUG_PIPELINE,
		PIPELINES_NUMBER
	};

	struct RenderPass {
		unsigned int            actualAttachmentDescriptionNumber;
		VkAttachmentDescription attachmentDescriptions[16];
		unsigned int            actualAttachmentReferenceNumber;
		VkAttachmentReference   attachmentReferences[16];
		unsigned int            actualSubpassDependencyNumber;
		VkSubpassDependency     subpassDependencies[8];
	};
	
	struct VK_Image {
		VkImage image;
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
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkVertexInputBindingDescription bindingDescription;
		std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions;
		unsigned int                    actualLinkedDescriptorSetsNumber;
		static constexpr unsigned int   maximumLinkedDescriptorSetDS = 32;
		unsigned int                    linkedDescriptorSetIDs[maximumLinkedDescriptorSetDS];
	};

	struct GPUBuffer {
		VkBuffer buffer;
		VkDeviceMemory deviceMemory;
	};

	union Descriptor {
		Descriptor() {};
		~Descriptor() {};
		
		GPUBuffer* GPUBuffer;
		VK_Image* GPUImage;
	};
	
	struct VulkanVertexLayout {
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
