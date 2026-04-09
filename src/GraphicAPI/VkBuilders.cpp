#include "GraphicAPI/RenderData.hpp"
#include "GraphicAPI/RenderConfig.hpp"
#include "VkStructs.hpp"

namespace GLVM::core {
	void descriptorSetBuilder() {
		static unsigned int descriptorBindingsIdCounter = 0;              ///< Counts ds bindings indexes inside ds
		static unsigned int descriptorSetsBindigOffsetCounter = 0;        ///< Counts host data ds
		static unsigned int globalDescriptorsOffset = 0;                  ///< Counts offsets data descriptors
		
		for( unsigned int dsCounter = 0; dsCounter < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++dsCounter ) {
//			std::cout << "ENUM NUM: " << dsCounter << std::endl;
			descriptorSetsConfig[dsCounter].descriptorSetOffset = descriptorSetsBindigOffsetCounter;                  ///< Offset for indexing inside descriptorSetsChunks
			descriptorSetsBindigOffsetCounter += descriptorSetsConfig[dsCounter].hostDescriptorNumber;
//			std::cout << "pravilni idealni offset: " << descriptorSetsConfig[dsCounter].descriptorSetOffset << std::endl;
			for( unsigned int bindingsIdCounter = 0; bindingsIdCounter < descriptorSetsConfig[dsCounter].actualLinkedDescriptorBindingsNumber; ++bindingsIdCounter ) {
				descriptorBindingsConfig[descriptorBindingsIdCounter + bindingsIdCounter].globalDescriptorOffset = globalDescriptorsOffset;     ///< Global offset for discriptors inside ds binding
				descriptorSetsConfig[dsCounter].descriptorsBindingsIDs[bindingsIdCounter] = descriptorBindingsIdCounter + bindingsIdCounter;    ///< Index for ds bindings inside ds
				if( descriptorBindingsConfig[descriptorBindingsIdCounter + bindingsIdCounter].vkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {
					for( unsigned int descriptorCounter = 0; descriptorCounter < descriptorBindingsConfig[descriptorBindingsIdCounter + bindingsIdCounter].shaderDescriptorsNumber; ++descriptorCounter ) {
//						std::cout << "allocate ubo" << std::endl;
						GPUDescriptors.Push( {} );
						GPUDescriptors[ GPUDescriptors.GetSize() - 1].GPUBuffer = new GPUBuffer;
						++globalDescriptorsOffset;
					}
				} else if ( descriptorBindingsConfig[descriptorBindingsIdCounter + bindingsIdCounter].vkType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ) {
					for( unsigned int descriptorCounter = 0; descriptorCounter < descriptorBindingsConfig[descriptorBindingsIdCounter + bindingsIdCounter].shaderDescriptorsNumber; ++descriptorCounter ) {
//						std::cout << "allocate texture" << std::endl;
						GPUDescriptors.Push( {} );
						GPUDescriptors[ GPUDescriptors.GetSize() - 1].GPUImage = new VK_Image;
						++globalDescriptorsOffset;
					}
				}
			}
			descriptorBindingsIdCounter += descriptorSetsConfig[dsCounter].actualLinkedDescriptorBindingsNumber;
		}
		descriptorSetsChunks.Resize( descriptorSetsBindigOffsetCounter );
	}

	void pipelineBuilder() {
		static unsigned int descriptorSetsLayoutIdCounter = 0;

		for( unsigned int pipelineCounter = 0; pipelineCounter < SpecificPipeline::PIPELINES_NUMBER; ++pipelineCounter ) {
//			std::cout << "number of ds: " << pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber << std::endl;
			for( unsigned int linkedDSLayoutConter = 0; linkedDSLayoutConter < pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber; ++linkedDSLayoutConter ) {
//				std::cout << "next id: " << descriptorSetsLayoutIdCounter + linkedDSLayoutConter << std::endl;
				pipelineConfigs[pipelineCounter].linkedDescriptorSetIDs[linkedDSLayoutConter] = descriptorSetsLayoutIdCounter + linkedDSLayoutConter;
			}
			descriptorSetsLayoutIdCounter += pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber;
		}
	}

	void renderPassesBuilder() {
		renderPasses.Resize( SpecificPipeline::PIPELINES_NUMBER );
	}
}; // GLVM::core
