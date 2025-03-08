#ifndef PROCEDURAL_LEVEL_GENEGRATING_SYSTEM
#define PROCEDURAL_LEVEL_GENEGRATING_SYSTEM

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "ISystem.hpp"
#include <cmath>
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
		unsigned int nextLevelTransitionDirection = 0;
		unsigned int previousIterationTransitionBridgeDirection = 0;
		struct {
			float positive_x = -MAXFLOAT;
			float negative_x = MAXFLOAT;
			float positive_y = -MAXFLOAT;
			float negative_y = MAXFLOAT;
			float positive_z = -MAXFLOAT;
			float negative_z = MAXFLOAT;
		} coordinateMaximumValuePerDirection;                                  ///< contain maximum coordinate value in every direction
		
		core::vector<ecs::components::MeshHandle> meshHandlers;
		core::vector<ecs::TextureHandle> textureHandlers;

        std::vector<core::vector<core::Vertex>> levelGeneratedVertices;
        std::vector<std::vector<uint32_t>> levelGeneratedIndices;              ///< wavefront.obj indices
		MeshAxisLimitingValues meshAxisLimitingValues;                         /// keep axis liniting values for every exis per mesh in current iteration while initializing wavefrontobj and gltf
		
		void Update();
		void generateLevel( const unsigned int half_x_rand, const unsigned int half_z_rand, const unsigned int half_y_rand,
							const float transitionBridgeHalfWidth, const float transitionBridgeHalfHeight );
		void generateTransitionBridge( const unsigned int half_x_rand, const unsigned int half_y_rand, const unsigned int half_z_rand,
									   const float transitionBridgeHalfWidth, const float transitionBridgeHalfHeight );
		void setCoordinateMaximumValuePerDirection(vec3 position, float half_x, float half_y, float half_z);
		bool checkCollisionIntersectionWithMaximumCoordinates(vec3 position, float half_x, float half_y, float half_z);
	};
}

#endif 
