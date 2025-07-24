#include "GraphicAPI/RenderData.hpp"
#include "VkStructs.hpp"

namespace GLVM::core {
	void descriptorSetBuilder() {
		descriptorSetsChunks.Resize( DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER );
	}
}; // GLVM::core
