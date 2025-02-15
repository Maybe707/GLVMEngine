#ifndef PROCEDURAL_LEVEL_GENEGRATING_SYSTEM
#define PROCEDURAL_LEVEL_GENEGRATING_SYSTEM

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "ISystem.hpp"
#include <map>
#include <random>

namespace GLVM::core
{
	class ProceduralLevelGeneratingSystem : public ecs::ISystem
	{
	public:
		unsigned int levelNubmer = 0;
		bool bredoFlag = false;
		
		core::vector<ecs::components::MeshHandle> meshHandlers;
		core::vector<ecs::TextureHandle> textureHandlers;
		
		void Update();
	};
}

#endif 
