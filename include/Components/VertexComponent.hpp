#ifndef VERTEX_COMPONENT
#define VERTEX_COMPONENT

// #include <vector>
// #include <iostream>
// #include "GraphicAPI/Vulkan.hpp"

#include <cstdint>

namespace GLVM::ecs::components
{
	struct MeshHandle {
		uint32_t id;
	};
	
	struct mesh
	{
        unsigned int id;
	};
}

#endif
