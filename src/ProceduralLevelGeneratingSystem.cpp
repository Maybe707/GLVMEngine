#include <ProceduralLevelGeneratingSystem.hpp>
#include <cmath>
#include <cstdint>
#include <random>
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "ArchetypeECS/ArchetypeEntityManager.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Archetypes/LevelChunkArchetype.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Constants.hpp"
#include "Engine.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "Vector.hpp"

namespace GLVM::core
{
	void ProceduralLevelGeneratingSystem::Update() {
		using namespace GLVM;
		namespace cm  = GLVM::ecs::components;
		namespace arch = GLVM::ecs::arch;
		core::Engine* GLVM = core::Engine::GetInstance();

		/// New arch ECS
		arch::ArchetypeEntityManager* archEntityManager = arch::ArchetypeEntityManager::getInstance();
		
		while ( levelNubmer < 5 ) {
			core::vector<core::Vertex> nextLevel;
			std::vector<uint32_t> indices;
			core::vector<core::Vertex> transitionBridgeVertices;
			std::vector<uint32_t> transitionBridgeIndices;

			if ( levelNubmer < 5 ) {
				std::random_device rd;
				std::mt19937 mersenne(rd());
				std::uniform_int_distribution<int> distCurrentLevel_y(1, 1);
				unsigned int half_y_rand = distCurrentLevel_y(mersenne);
				std::uniform_int_distribution<int> distCurrentLevel_x_z(8, 16);
				unsigned int half_x_rand = distCurrentLevel_x_z(mersenne);
				unsigned int half_z_rand = distCurrentLevel_x_z(mersenne);

				constexpr float transitionBridgeHalfWidth  = 0.5f;              ///< Need to move on half
				constexpr float transitionBridgeHalfHeight = 1.0f;
				if ( levelNubmer != 0 ) {                                               ///< On first iteration we dont need to define where locate current level depends on previousTransitionBridge
					generateLevel( half_x_rand, half_y_rand, half_z_rand, transitionBridgeHalfWidth, transitionBridgeHalfHeight );
				} else {
					// Set to first level maximum values
					coordinateMaximumValuePerDirection.lowest_x  = currentLevelPosition[0] - half_x_rand;
					coordinateMaximumValuePerDirection.highest_x = currentLevelPosition[0] + half_x_rand;
					coordinateMaximumValuePerDirection.lowest_y  = currentLevelPosition[1] - half_y_rand;
					coordinateMaximumValuePerDirection.highest_y = currentLevelPosition[1] + half_y_rand;
					coordinateMaximumValuePerDirection.lowest_z  = currentLevelPosition[2] - half_z_rand;
					coordinateMaximumValuePerDirection.highest_z = currentLevelPosition[2] + half_z_rand;
				}
				
				generateTransitionBridge( half_x_rand, half_y_rand, half_z_rand, transitionBridgeHalfWidth, transitionBridgeHalfHeight );

				for ( unsigned int i = 0; i < 36; ++i )
					indices.push_back(boxIndicesForIndexBuffer[i]);

				allMeshMaxAbsoluteValues.Push({});
				meshAxisLimitingValues.setToDefaultValues();

				makeCubeObjectVertices( { -1, -1, -1, -1 }, { 1, 1, 1, 1 }, half_x_rand, half_y_rand, half_z_rand, nextLevel );

				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x = (meshAxisLimitingValues.highest_x - meshAxisLimitingValues.lowest_x) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y = (meshAxisLimitingValues.highest_y - meshAxisLimitingValues.lowest_y) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z = (meshAxisLimitingValues.highest_z - meshAxisLimitingValues.lowest_z) / 2.0f;

				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].origin_offset_x = (meshAxisLimitingValues.highest_x + meshAxisLimitingValues.lowest_x) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].origin_offset_y = (meshAxisLimitingValues.highest_y + meshAxisLimitingValues.lowest_y) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].origin_offset_z = (meshAxisLimitingValues.highest_z + meshAxisLimitingValues.lowest_z) / 2.0f;
				

				[[maybe_unused]] cm::MeshHandle gameLevelMeshHandle = GLVM->LoadMesh();
				arch::entity gameLevelChunkEntity = archEntityManager->createEntity();

				cachedLevelChunkArchNumber = 0;
				/// Search and cache one time for LevelChunkArch
				arch::world.searchCacheArchetypes( requiredMask, &archView.cachedLevelChunkArch, cachedLevelChunkArchNumber );
				
				arch::world.addEntityToArchetype( gameLevelChunkEntity, archView.cachedLevelChunkArch );
				arch::EntityLocation gameLevelChunkLocation = arch::world.entityLocations[arch::getId( gameLevelChunkEntity )];
				
				arch::LevelChunkArchetype* levelChunkArch = static_cast<arch::LevelChunkArchetype*>(gameLevelChunkLocation.arch);
				const uint32_t gameLevelChunkIndex = gameLevelChunkLocation.index;
				ecs::TextureHandle gameLevelTexture = textureHandlers[2];
				levelChunkArch->transforms[gameLevelChunkIndex] = { .position = currentLevelPosition, .scale = 1.0f };
				levelChunkArch->materials[gameLevelChunkIndex]  = { .diffuseTextureID_ = gameLevelTexture, .specularTextureID_ = gameLevelTexture, .ambient = { 0.05f, 0.05f, 0.0f }, .shininess = 128.0f * 0.078125f };
				levelChunkArch->meshes[gameLevelChunkIndex].handle = gameLevelMeshHandle;
				
				for ( unsigned int i = 0; i < 36; ++i )
					transitionBridgeIndices.push_back(boxIndicesForIndexBuffer[i]);

				allMeshMaxAbsoluteValues.Push({});
				meshAxisLimitingValues.setToDefaultValues();

				float half_x = 0.0f;
				float half_y = half_y_rand;
				float half_z = 0.0f;
				if ( nextLevelTransitionDirection == 1 || nextLevelTransitionDirection == 3 ) {
					half_x = transitionBridgeHalfWidth;
					half_z = transitionBridgeHalfHeight;
				} else if ( nextLevelTransitionDirection == 2 || nextLevelTransitionDirection == 4 ) {
					half_x = transitionBridgeHalfHeight;
					half_z = transitionBridgeHalfWidth;
				}
				makeCubeObjectVertices( { -1, -1, -1, -1 }, { 1, 1, 1, 1 }, half_x, half_y, half_z, transitionBridgeVertices );

				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x = (meshAxisLimitingValues.highest_x - meshAxisLimitingValues.lowest_x) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y = (meshAxisLimitingValues.highest_y - meshAxisLimitingValues.lowest_y) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z = (meshAxisLimitingValues.highest_z - meshAxisLimitingValues.lowest_z) / 2.0f;

				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].origin_offset_x = (meshAxisLimitingValues.highest_x + meshAxisLimitingValues.lowest_x) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].origin_offset_y = (meshAxisLimitingValues.highest_y + meshAxisLimitingValues.lowest_y) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].origin_offset_z = (meshAxisLimitingValues.highest_z + meshAxisLimitingValues.lowest_z) / 2.0f;


				[[maybe_unused]] cm::MeshHandle transitionBridgeMeshHandle = GLVM->LoadMesh();
				arch::entity transitionBridgeEntity = archEntityManager->createEntity();
				arch::world.addEntityToArchetype( transitionBridgeEntity, archView.cachedLevelChunkArch );

				arch::EntityLocation transitionBridgeLocation = arch::world.entityLocations[arch::getId( transitionBridgeEntity )];

				arch::LevelChunkArchetype* transitionBridgeArch = static_cast<arch::LevelChunkArchetype*>(transitionBridgeLocation.arch);
				const uint32_t transitionBridgeIndex = transitionBridgeLocation.index;
				ecs::TextureHandle transitionBridgeTexture = textureHandlers[3];
				transitionBridgeArch->transforms[transitionBridgeIndex] = { .position = transitionBridgePosition, .scale = 1.0f };
				transitionBridgeArch->materials[transitionBridgeIndex]  = { .diffuseTextureID_ = transitionBridgeTexture, .specularTextureID_ = transitionBridgeTexture, .ambient = { 0.05f, 0.05f, 0.0f }, .shininess = 128.0f * 0.078125f };
				transitionBridgeArch->meshes[transitionBridgeIndex].handle = transitionBridgeMeshHandle;
				
				++levelNubmer;
			}
			levelGeneratedVertices.push_back(nextLevel);
			levelGeneratedIndices.push_back(indices);

			levelGeneratedVertices.push_back(transitionBridgeVertices);
			levelGeneratedIndices.push_back(transitionBridgeIndices);
			
			bredoFlag = true;
		}
	}

	void ProceduralLevelGeneratingSystem::generateLevel( const unsigned int half_x_rand, const unsigned int half_y_rand, const unsigned int half_z_rand,
														 const float transitionBridgeHalfWidth, const float transitionBridgeHalfHeight ) {
		std::random_device rd;
		std::mt19937 mersenne(rd());
		unsigned int previousTransitionBridgeAnchorPoint   = 0;
		bool validLevel = false;
		while ( !validLevel ) {
			switch( previousIterationTransitionBridgeDirection ) {
			case 1:
			{
				std::uniform_int_distribution<int> distPreviousTransitionBridgeAnchorPoint( 0, half_x_rand * 2 - 1 );
				previousTransitionBridgeAnchorPoint = distPreviousTransitionBridgeAnchorPoint(mersenne);
				currentLevelPosition[0] = transitionBridgePosition[0] - half_x_rand + transitionBridgeHalfWidth
					+ previousTransitionBridgeAnchorPoint;
				currentLevelPosition[2] = transitionBridgePosition[2] + half_z_rand + transitionBridgeHalfHeight;
			}
			break;
			case 2:
			{
				std::uniform_int_distribution<int> distPreviousTransitionBridgeAnchorPoint( 0, half_z_rand * 2 - 1 );
				previousTransitionBridgeAnchorPoint = distPreviousTransitionBridgeAnchorPoint(mersenne);
				currentLevelPosition[2] = transitionBridgePosition[2] - half_z_rand + transitionBridgeHalfWidth
					+ previousTransitionBridgeAnchorPoint;
				currentLevelPosition[0] = transitionBridgePosition[0] + half_x_rand + transitionBridgeHalfHeight;
			}
			break;
			case 3:
			{
				std::uniform_int_distribution<int> distPreviousTransitionBridgeAnchorPoint( 0, half_x_rand * 2 - 1 );
				previousTransitionBridgeAnchorPoint = distPreviousTransitionBridgeAnchorPoint(mersenne);
				currentLevelPosition[0] = transitionBridgePosition[0] - half_x_rand + transitionBridgeHalfWidth
					+ previousTransitionBridgeAnchorPoint;
				currentLevelPosition[2] = transitionBridgePosition[2] - half_z_rand - transitionBridgeHalfHeight;
			}
			break;
			case 4:
			{
				std::uniform_int_distribution<int> distPreviousTransitionBridgeAnchorPoint( 0, half_z_rand * 2 - 1 );
				previousTransitionBridgeAnchorPoint = distPreviousTransitionBridgeAnchorPoint(mersenne);
				currentLevelPosition[2] = transitionBridgePosition[2] - half_z_rand + transitionBridgeHalfWidth
					+ previousTransitionBridgeAnchorPoint;
				currentLevelPosition[0] = transitionBridgePosition[0] - half_x_rand - transitionBridgeHalfHeight;
			}
			break;
			}

			if ( checkCollisionIntersectionWithMaximumCoordinates(currentLevelPosition, half_x_rand, half_y_rand, half_z_rand ) ) {
				std::cout << "LEVEL COLLITION DETECTED" << std::endl;
				previousIterationTransitionBridgeDirection = (4 + previousIterationTransitionBridgeDirection) % 4 + 1;
			} else {
				coordinateMaximumValuePerDirection.comparePerDirectionAndSetToMaximumValueByModule( currentLevelPosition, (float)half_x_rand, (float)half_y_rand, (float)half_z_rand );
				validLevel = true;
			}
		}
		currentLevelPosition[1] = 0.0f;
	}

	void ProceduralLevelGeneratingSystem::generateTransitionBridge( const unsigned int levelHalfX, const unsigned int levelHalfY, const unsigned int levelHalfZ,
																	const float transitionBridgeHalfWidth, const float transitionBridgeHalfHeight ) {
		std::random_device rd;
		std::mt19937 mersenne(rd());
		std::uniform_int_distribution<int> distNextLevelTransitionDirection(1, 4);                       ///< 1 - north, 2 - east, 3 - south, 4 - west
		nextLevelTransitionDirection = distNextLevelTransitionDirection(mersenne);          ///< randomly chose direction in where next level will appeared
		unsigned int transitionBridgeAnchorPoint  = 0;
		float transitionBridgeOffset_x  = 0.0f;
		float transitionBridgeOffset_z  = 0.0f;
		bool validTransitionBridge = false;
		while ( !validTransitionBridge ) {
			if ( nextLevelTransitionDirection == 1 || nextLevelTransitionDirection == 3 ) {     ///< Choose up (1) or down (3) insert point direction
				std::uniform_int_distribution<int> distTransitionBridgeAnchorPoint( 0, levelHalfX * 2 - 1 );    ///< In what point we connect next transition bridge to current level
				transitionBridgeAnchorPoint = distTransitionBridgeAnchorPoint(mersenne);
				/// Summarize most left position with random value of point where transition bridge will be insert
				transitionBridgeOffset_x = -(float)levelHalfX + (float)transitionBridgeAnchorPoint; 
				if ( nextLevelTransitionDirection == 1 ) {
					transitionBridgeOffset_z = levelHalfZ;            ///< Move to the bottom level edge
					transitionBridgePosition = { currentLevelPosition[0] + transitionBridgeOffset_x + transitionBridgeHalfWidth, (float)levelHalfY,
						currentLevelPosition[2] + transitionBridgeOffset_z + transitionBridgeHalfHeight };
				} else {
					transitionBridgeOffset_z = -(float)levelHalfZ;    ///< Move to the upper level edge
					transitionBridgePosition = { currentLevelPosition[0] + transitionBridgeOffset_x + transitionBridgeHalfWidth, (float)levelHalfY,
						currentLevelPosition[2] + transitionBridgeOffset_z - transitionBridgeHalfHeight };
				}
			} else if ( nextLevelTransitionDirection == 2 || nextLevelTransitionDirection == 4 ) {   ///< Choose left (2) or right (4) insert point direction
				std::uniform_int_distribution<int> distTransitionBridgeAnchorPoint( 0, levelHalfZ * 2 - 1 );    ///< In what point we connect next transition bridge to current level
				transitionBridgeAnchorPoint = distTransitionBridgeAnchorPoint(mersenne);
				/// Summarize forwardmost position with random value of point where transition bridge will be insert
				transitionBridgeOffset_z    = -(float)levelHalfZ + (float)transitionBridgeAnchorPoint;
				if ( nextLevelTransitionDirection == 2 ) {
					transitionBridgeOffset_x = levelHalfX;            ///< Move to the right level edge
					transitionBridgePosition = { currentLevelPosition[0] + transitionBridgeOffset_x + transitionBridgeHalfHeight, (float)levelHalfY,
						currentLevelPosition[2] + transitionBridgeOffset_z + transitionBridgeHalfWidth };
				} else {
					transitionBridgeOffset_x = -(float)levelHalfX;    ///< Move to the left level edge
					transitionBridgePosition = { currentLevelPosition[0] + transitionBridgeOffset_x - transitionBridgeHalfHeight, (float)levelHalfY,
						currentLevelPosition[2] + transitionBridgeOffset_z + transitionBridgeHalfWidth };
				}
			}

			float width = 0;
			float height = 0;
			/// chose transitionBridgeHalfWidth as X and transitionBridgeHalfHeight as Z
			if ( nextLevelTransitionDirection == 1 || nextLevelTransitionDirection == 3 ) { 
				width = transitionBridgeHalfWidth;
				height = transitionBridgeHalfHeight;
			}
			/// chose transitionBridgeHalfWidth as Z and transitionBridgeHalfHeight as X
			if ( nextLevelTransitionDirection == 2 || nextLevelTransitionDirection == 4 ) {
				width = transitionBridgeHalfHeight;
				height = transitionBridgeHalfWidth;
			}
					
			if ( checkCollisionIntersectionWithMaximumCoordinates(transitionBridgePosition, width, levelHalfY, height ) ) {
				/// Need to choose another direction if we got collided with level
				nextLevelTransitionDirection = (4 + nextLevelTransitionDirection) % 4 + 1;
			} else {
				/// Setting up bounds for all levels
				coordinateMaximumValuePerDirection.comparePerDirectionAndSetToMaximumValueByModule( transitionBridgePosition, (float)width, (float)levelHalfY, (float)height );
				validTransitionBridge = true;
			}
		}
		transitionBridgePosition[1]                = currentLevelPosition[1];
		previousIterationTransitionBridgeDirection = nextLevelTransitionDirection;
	}

	void ProceduralLevelGeneratingSystem::makeCubeObjectVertices( vec4 joinIndices, vec4 weights, float half_x, float half_y, float half_z,
																  core::vector<core::Vertex>& destinationVerticesContainer ) {
		unsigned int cube_vertices = 8;
		for ( unsigned int i = 0; i < cube_vertices; ++i ) {
			SVertex vertex;
					
			switch( i ) {
			case 0:
				vertex[0] = half_x;
				vertex[1] = half_y;
				vertex[2] = half_z;
				break;
			case 1:
				vertex[0] = -(float)half_x;
				vertex[1] = half_y;
				vertex[2] = half_z;
				break;			
			case 2:
				vertex[0] = -(float)half_x;
				vertex[1] = -(float)half_y;
				vertex[2] = half_z;
				break;			
			case 3:
				vertex[0] = half_x;
				vertex[1] = -(float)half_y;
				vertex[2] = half_z;
				break;
			case 4:
				vertex[0] = half_x;
				vertex[1] = half_y;
				vertex[2] = -(float)half_z;
				break;
			case 5:
				vertex[0] = -(float)half_x;
				vertex[1] = half_y;
				vertex[2] = -(float)half_z;
				break;			
			case 6:
				vertex[0] = -(float)half_x;
				vertex[1] = -(float)half_y;
				vertex[2] = -(float)half_z;
				break;			
			case 7:
				vertex[0] = half_x;
				vertex[1] = -(float)half_y;
				vertex[2] = -(float)half_z;
				break;			
			}

			meshAxisLimitingValues.comparePerDirectionAndSetToMaximumValueByModule( vertex );
					
			SVertex normal;
			normal[0] = 0;
			normal[1] = 1;
			normal[2] = 0;
			SVertex texture;
			texture[0] = 0;
			texture[1] = 1;

			destinationVerticesContainer.Push({{vertex[0], vertex[1], vertex[2]},
											   {normal[0], normal[1], normal[2]},
											   {texture[0], texture[1]},
											   {joinIndices[0], joinIndices[1], joinIndices[2], joinIndices[3]},
											   {weights[0], weights[1], weights[2], weights[3]}});
		}
	}
	
	bool ProceduralLevelGeneratingSystem::checkCollisionIntersectionWithMaximumCoordinates(vec3 position, float half_x, float half_y, float half_z) {
		if ( position[0] + half_x > coordinateMaximumValuePerDirection.lowest_x &&
			 position[0] - half_x < coordinateMaximumValuePerDirection.highest_x &&
			 position[1] + half_y > coordinateMaximumValuePerDirection.lowest_y &&
			 position[1] - half_y < coordinateMaximumValuePerDirection.highest_y &&
			 position[2] + half_z > coordinateMaximumValuePerDirection.lowest_z &&
			 position[2] - half_z < coordinateMaximumValuePerDirection.highest_z ) {
			return true;
		} else {
			return false;
		}
	}
}
