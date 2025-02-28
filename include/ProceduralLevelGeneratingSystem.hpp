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
		unsigned int levelNubmer          = 0;
		bool bredoFlag                    = false;
		unsigned int previous_half_x_rand = 0;
		unsigned int previous_half_z_rand = 0;
//		vec3 previousLevelPosition        = { 5.0f, 0.0f, 15.0f };
		vec3 currentLevelPosition         = { 5.0f, 0.0f, 15.0f };
		vec3 transitionBridgePosition     = { 0.0f, 0.0f, 0.0f };
		unsigned int previousIterationTransitionBridgeDirection = 0;
		
		core::vector<ecs::components::MeshHandle> meshHandlers;
		core::vector<ecs::TextureHandle> textureHandlers;

        std::vector<core::vector<core::Vertex>> levelGeneratedVertices;
        std::vector<std::vector<uint32_t>> levelGeneratedIndices;                 ///< wavefront.obj indices
		MeshAxisLimitingValues meshAxisLimitingValues;                         /// keep axis liniting values for every exis per mesh in current iteration while initializing wavefrontobj and gltf
		
		void Update();
	};
}

#endif 
