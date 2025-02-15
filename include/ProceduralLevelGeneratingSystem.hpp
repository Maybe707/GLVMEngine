#ifndef PROCEDURAL_LEVEL_GENEGRATING_SYSTEM
#define PROCEDURAL_LEVEL_GENEGRATING_SYSTEM

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "ISystem.hpp"
#include <map>
#include <random>
#include "GraphicAPI/Vulkan.hpp"

namespace GLVM::core
{
	class ProceduralLevelGeneratingSystem : public ecs::ISystem
	{
	public:
		unsigned int levelNubmer = 0;
		bool bredoFlag = false;
		
		core::vector<ecs::components::MeshHandle> meshHandlers;
		core::vector<ecs::TextureHandle> textureHandlers;

        std::vector<core::vector<core::Vertex>> levelGeneratedVertices;
        std::vector<std::vector<uint32_t>> levelGeneratedIndices;                 ///< wavefront.obj indices
		
		void Update();
	};
}

#endif 
