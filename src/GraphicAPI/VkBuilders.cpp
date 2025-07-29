#include "GraphicAPI/RenderData.hpp"
#include "GraphicAPI/RenderConfig.hpp"
#include "VkStructs.hpp"

namespace GLVM::core {
	void descriptorSetBuilder() {
		static unsigned int descriptorBindingsIdCounter = 0;
		static unsigned int descriptorSetsBindigOffsetCounter = 0;
		
		for( unsigned int dsCounter = 0; dsCounter < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++dsCounter ) {
			descriptorSetsConfig[dsCounter].descriptorSetOffset = descriptorSetsBindigOffsetCounter;
			descriptorSetsBindigOffsetCounter += descriptorSetsConfig[dsCounter].hostDescriptorNumber;
			std::cout << "pravilni idealni offset: " << descriptorSetsConfig[dsCounter].descriptorSetOffset << std::endl;
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
		descriptorSetsChunks.Resize( descriptorSetsBindigOffsetCounter );
	}

	void pipelineBuilder() {
		static unsigned int descriptorSetsLayoutIdCounter = 0;

		for( unsigned int pipelineCounter = 0; pipelineCounter < SpecificPipeline::PIPELINES_NUMBER; ++pipelineCounter ) {
			std::cout << "number of ds: " << pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber << std::endl;
			for( unsigned int linkedDSLayoutConter = 0; linkedDSLayoutConter < pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber; ++linkedDSLayoutConter ) {
				std::cout << "next id: " << descriptorSetsLayoutIdCounter + linkedDSLayoutConter << std::endl;
				pipelineConfigs[pipelineCounter].linkedDescriptorSetIDs[linkedDSLayoutConter] = descriptorSetsLayoutIdCounter + linkedDSLayoutConter;
			}
			descriptorSetsLayoutIdCounter += pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber;
		}
	}

	void renderPassesBuilder() {
		renderPasses.Resize( SpecificPipeline::PIPELINES_NUMBER );
	}
}; // GLVM::core
