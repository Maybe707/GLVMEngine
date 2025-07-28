#include "GraphicAPI/RenderData.hpp"
#include "GraphicAPI/RenderConfig.hpp"
#include "VkStructs.hpp"

namespace GLVM::core {
	void descriptorSetBuilder() {
		static unsigned int descriptorBindingsIdCounter = 0;
		
		descriptorSetsChunks.Resize( DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER );
		for( unsigned int dsCounter = 0; dsCounter < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++dsCounter ) {
			for( unsigned int bindingsIdCounter = 0; bindingsIdCounter < descriptorSetsConfig[dsCounter].actualLinkedDescriptorBindingsNumber; ++bindingsIdCounter ) {
				descriptorSetsConfig[dsCounter].descriptorsBindingsIDs[bindingsIdCounter] = descriptorBindingsIdCounter + bindingsIdCounter;
				if( descriptorBindingsConfig[descriptorBindingsIdCounter + bindingsIdCounter].vkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {
					GPUDescriptors.Push( {} );
					GPUDescriptors[ GPUDescriptors.GetSize() - 1].GPUBuffer = new GPUBuffer;
				} else if ( descriptorBindingsConfig[descriptorBindingsIdCounter + bindingsIdCounter].vkType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ) {
					GPUDescriptors.Push( {} );
					GPUDescriptors[ GPUDescriptors.GetSize() - 1].GPUImage = new VK_Image;
				}
			}
			descriptorBindingsIdCounter += descriptorSetsConfig[dsCounter].actualLinkedDescriptorBindingsNumber;
		}
	}

	void pipelineBuilder() {
		static unsigned int descriptorSetsLayoutIdCounter = 0;

		for( unsigned int pipelineCounter = 0; pipelineCounter < SpecificPipeline::PIPELINES_NUMBER; ++pipelineCounter ) {
			for( unsigned int linkedDSLayoutConter = 0; linkedDSLayoutConter < pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber; ++linkedDSLayoutConter ) {
				pipelineConfigs[pipelineCounter].linkedDescriptorSetIDs[linkedDSLayoutConter] = descriptorSetsLayoutIdCounter + linkedDSLayoutConter;
			}
			descriptorSetsLayoutIdCounter += pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber;
		}
	}

	void renderPassesBuilder() {
		renderPasses.Resize( SpecificPipeline::PIPELINES_NUMBER );
	}
}; // GLVM::core
