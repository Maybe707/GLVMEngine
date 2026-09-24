#include "GraphicAPI/RenderData.hpp"
#include "GraphicAPI/RenderConfig.hpp"
#include "VkStructs.hpp"
#include "typenames.hpp"

namespace GLVM::core {
	void descriptorSetBuilder() {
		static unsigned int DS_globalBindingsCounter = 0;                 ///< Counts descriptor set bindings indexes inside it
		static unsigned int DS_hostNumber = 0;                            ///< Counts host data ds
		static unsigned int globalDescriptorsOffset = 0;                  ///< Counts descriptors in GPUDescriptors array
		
		for( u32 dsCounter = 0; dsCounter < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++dsCounter ) {
			/// Offset for indexing inside descriptorSetsChunks
			DescriptorSet& descriptorSet = descriptorSetsConfig[dsCounter];
			descriptorSet.descriptorSetOffset = DS_hostNumber;  
			DS_hostNumber += descriptorSet.hostDescriptorNumber;
			
			for( u32 DS_localBindingsCounter = 0; DS_localBindingsCounter <
					 descriptorSet.actualLinkedDescriptorBindingsNumber; ++DS_localBindingsCounter ) {
				const u32 DS_sumBindingsCounter = DS_globalBindingsCounter + DS_localBindingsCounter;    ///< Current binding in descriptorBindingConfig *
				DescriptorBinding& binding =  descriptorBindingsConfig[DS_sumBindingsCounter];
				/// Global offset for discriptors inside ds binding
				binding.globalDescriptorOffset = globalDescriptorsOffset;
					
				/// Index for ds bindings inside ds
				descriptorSet.descriptorsBindingsIDs[DS_localBindingsCounter] = DS_sumBindingsCounter;    
				if( binding.vkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {
					for( u32 descriptorCounter = 0; descriptorCounter < binding.shaderDescriptorsNumber; ++descriptorCounter ) {
						GPUDescriptors.Push( {} );
						GPUDescriptors[ GPUDescriptors.GetSize() - 1].GPUBuffer = new GPUBuffer;
						++globalDescriptorsOffset;
					}
				} else if ( binding.vkType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ) {
					for( u32 descriptorCounter = 0; descriptorCounter < binding.shaderDescriptorsNumber; ++descriptorCounter ) {
						GPUDescriptors.Push( {} );
						GPUDescriptors[ GPUDescriptors.GetSize() - 1].GPUImage = new VK_Image;
						++globalDescriptorsOffset;
					}
				}
			}
			DS_globalBindingsCounter += descriptorSet.actualLinkedDescriptorBindingsNumber; ///< Set offset on every number of binding of current descriptor set *
		}
		descriptorSetsChunks.Resize( DS_hostNumber );
	}

	void pipelineBuilder() {
		static unsigned int descriptorSetsLayoutIdCounter = 0;
		for( unsigned int pipelineCounter = 0; pipelineCounter < SpecificPipeline::PIPELINES_NUMBER; ++pipelineCounter ) {
			for( unsigned int linkedDSLayoutCounter = 0; linkedDSLayoutCounter <
					 pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber; ++linkedDSLayoutCounter ) {
				pipelineConfigs[pipelineCounter].linkedDescriptorSetIDs[linkedDSLayoutCounter] = descriptorSetsLayoutIdCounter + linkedDSLayoutCounter;
			}
			descriptorSetsLayoutIdCounter += pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber;
		}
	}

	void renderPassesBuilder() {
		renderPasses.Resize( SpecificPipeline::PIPELINES_NUMBER );
	}
}; // GLVM::core
