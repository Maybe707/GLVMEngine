#include "GraphicAPI/RenderData.hpp"
#include "GraphicAPI/RenderConfig.hpp"
#include "VkStructs.hpp"
#include "typenames.hpp"

namespace GLVM::core {
	void descriptorSetBuilder(VulkanResources& resources) {
		unsigned int DS_globalBindingsCounter = 0;                 ///< Counts ds bindings indexes inside ds
		unsigned int DS_hostNumber = 0;        ///< Counts host data ds
		unsigned int globalDescriptorsOffset = 0;                  ///< Counts offsets data descriptors

		for( unsigned int dsCounter = 0; dsCounter < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++dsCounter ) {
			/// Offset for indexing inside resources.descriptorSetsChunks
			resources.descriptorSetsConfig[dsCounter].descriptorSetOffset = DS_hostNumber;
			DS_hostNumber += resources.descriptorSetsConfig[dsCounter].hostDescriptorNumber;

			for( unsigned int DS_localBindingsCounter = 0; DS_localBindingsCounter <
					 resources.descriptorSetsConfig[dsCounter].actualLinkedDescriptorBindingsNumber; ++DS_localBindingsCounter ) {
				const u32 DS_sumBindingsCounter = DS_globalBindingsCounter + DS_localBindingsCounter;
				/// Global offset for discriptors inside ds binding
				resources.descriptorBindingsConfig[DS_sumBindingsCounter].globalDescriptorOffset = globalDescriptorsOffset;

				/// Index for ds bindings inside ds
				resources.descriptorSetsConfig[dsCounter].descriptorsBindingsIDs[DS_localBindingsCounter] = DS_sumBindingsCounter;
				if( resources.descriptorBindingsConfig[DS_sumBindingsCounter].vkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {
					for( unsigned int descriptorCounter = 0; descriptorCounter <
							 resources.descriptorBindingsConfig[DS_sumBindingsCounter].shaderDescriptorsNumber; ++descriptorCounter ) {
						resources.GPUDescriptors.Push( {} );
						resources.buffers.emplace_back();
                        resources.GPUDescriptors[resources.GPUDescriptors.GetSize() - 1].GPUBuffer = &resources.buffers.back();
						++globalDescriptorsOffset;
					}
				} else if ( resources.descriptorBindingsConfig[DS_sumBindingsCounter].vkType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ) {
					for( unsigned int descriptorCounter = 0; descriptorCounter <
							 resources.descriptorBindingsConfig[DS_sumBindingsCounter].shaderDescriptorsNumber; ++descriptorCounter ) {
						resources.GPUDescriptors.Push( {} );
						resources.images.emplace_back();
                        resources.GPUDescriptors[resources.GPUDescriptors.GetSize() - 1].GPUImage = &resources.images.back();
						++globalDescriptorsOffset;
					}
				}
			}
			DS_globalBindingsCounter += resources.descriptorSetsConfig[dsCounter].actualLinkedDescriptorBindingsNumber;
		}
		resources.descriptorSetsChunks.Resize( DS_hostNumber );
	}

	void pipelineBuilder(VulkanResources& resources) {
		unsigned int descriptorSetsLayoutIdCounter = 0;
		for( unsigned int pipelineCounter = 0; pipelineCounter < SpecificPipeline::PIPELINES_NUMBER; ++pipelineCounter ) {
			for( unsigned int linkedDSLayoutCounter = 0; linkedDSLayoutCounter <
					 resources.pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber; ++linkedDSLayoutCounter ) {
				resources.pipelineConfigs[pipelineCounter].linkedDescriptorSetIDs[linkedDSLayoutCounter] = descriptorSetsLayoutIdCounter + linkedDSLayoutCounter;
			}
			descriptorSetsLayoutIdCounter += resources.pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber;
		}
	}

	void renderPassesBuilder(VulkanResources& resources) {
		resources.renderPasses.Resize( SpecificPipeline::PIPELINES_NUMBER );
	}
}; // GLVM::core
