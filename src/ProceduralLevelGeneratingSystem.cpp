#include <ProceduralLevelGeneratingSystem.hpp>
#include <cmath>
#include <random>
#include "Components/ColliderComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Engine.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "Vector.hpp"

namespace GLVM::core
{
	void ProceduralLevelGeneratingSystem::Update() {
		using namespace GLVM;
		namespace cm  = GLVM::ecs::components;
		core::Engine* GLVM = core::Engine::GetInstance();
		ecs::EntityManager   * EntityManager     = ecs::EntityManager::GetInstance();
		ecs::ComponentManager* ComponentManager  = ecs::ComponentManager::GetInstance();

		while ( levelNubmer < 5 ) {
			core::vector<core::Vertex> nextLevel;
			std::vector<uint32_t> indices;
//			std::cout << "size of all mesh container in proc gen 0: " << allMeshMaxAbsoluteValues.GetSize() << std::endl;
			core::vector<core::Vertex> transitionBridgeVertices;
			std::vector<uint32_t> transitionBridgeIndices;

			if ( levelNubmer < 5 ) {
				std::random_device rd;
				std::mt19937 mersenne(rd());
				std::uniform_int_distribution<int> distCurrentLevel_y(1, 1);
				unsigned int half_y_rand = distCurrentLevel_y(mersenne);
				std::uniform_int_distribution<int> distCurrentLevel_x_z(4, 8);
				unsigned int half_x_rand = distCurrentLevel_x_z(mersenne);
				unsigned int half_z_rand = distCurrentLevel_x_z(mersenne);

				constexpr float transitionBridgeHalfWidth  = 0.5f;              ///< Need to move on half
				constexpr float transitionBridgeHalfHeight = 1.0f;
				if ( levelNubmer != 0 ) {                                               ///< On first iteration we dont need to define where locate current level depends on previousTransitionBridge
					generateLevel( half_x_rand, half_y_rand, half_z_rand, transitionBridgeHalfWidth, transitionBridgeHalfHeight );
				}
				
				generateTransitionBridge( half_x_rand, half_y_rand, half_z_rand, transitionBridgeHalfWidth, transitionBridgeHalfHeight );
				
				// std::cout << "prev dir: " << previousIterationTransitionBridgeDirection << std::endl;
				// std::cout << "next dir: " << nextLevelTransitionDirection << std::endl;
				// std::cout << "transition bridge position: " << transitionBridgePosition << std::endl;

				// std::cout << "width: " << width << std::endl;
				// std::cout << "height: " << height << std::endl;
//				setCoordinateMaximumValuePerDirection(transitionBridgePosition, (float)width, (float)half_y_rand, (float)height);
				// std::cout << "maximum positive x: " << coordinateMaximumValuePerDirection.positive_x << std::endl;
				// std::cout << "maximum negative x: " << coordinateMaximumValuePerDirection.negative_x << std::endl;
				// std::cout << "maximum positive y: " << coordinateMaximumValuePerDirection.positive_y << std::endl;
				// std::cout << "maximum negative y: " << coordinateMaximumValuePerDirection.negative_y << std::endl;
				// std::cout << "maximum positive z: " << coordinateMaximumValuePerDirection.positive_z << std::endl;
				// std::cout << "maximum negative z: " << coordinateMaximumValuePerDirection.negative_z << std::endl;

//				std::cout << "transition bridge position: " << transitionBridgePosition << std::endl;

//				std::cout << "current level position: " << currentLevelPosition << std::endl;
				// if ( levelNubmer == 0 ) {
				// 	currentLevelPosition = transitionBridgePosition;
				// } else {
				// 	currentLevelPosition = { currentLevelPosition[0] + 
				// }
				// unsigned int transitionBridgeAnchorPoint  
				// switch( nextLevelTransitionDirection ) {
				// case 1:
					
				// }
				// std::uniform_int_distribution<int> distLevelPosition_x(previousLevelPosition[0] + previous_half_x_rand + half_x_rand + 2,
				// 													   previousLevelPosition[0] + previous_half_x_rand + half_x_rand + 6);
				// std::uniform_int_distribution<int> distLevelPosition_z(previousLevelPosition[2] + previous_half_z_rand + half_z_rand + 2,
				// 													   previousLevelPosition[2] + previous_half_z_rand + half_z_rand + 6);
				
				// if ( levelNubmer == 0 ) {
				// 	currentLevelPosition = previousLevelPosition;
				// } else {
				// 	currentLevelPosition = { (float)distLevelPosition_x(mersenne), 0.0f, (float)distLevelPosition_z(mersenne) };
				// 	previousLevelPosition = currentLevelPosition;
				// }
				
				// previous_half_x_rand     = half_x_rand;
				// previous_half_z_rand     = half_z_rand;

				// unsigned int half_x_rand = 0;
				// unsigned int half_y_rand = 0;
				// unsigned int half_z_rand = 0;
				// if ( levelNubmer == 0 ) {
				// 	half_x_rand = 2;
				// 	half_y_rand = 3;
				// 	half_z_rand = 2;
				// } else if ( levelNubmer == 1 ) {
				// 	half_x_rand = 1;
				// 	half_y_rand = 2;
				// 	half_z_rand = 1;
				// } else if ( levelNubmer == 2 ) {
				// 	half_x_rand = 1;
				// 	half_y_rand = 3;
				// 	half_z_rand = 2;
				// }
				
				// vec3 randomDirection = {};
				// switch( random ) {
				// case 0:
				// 	randomDirection = vec3( 50.0f, -60.0f, 0.0f, 0.0 );
				// 	break;
				// case 1:
				// 	randomDirection = vec3( -50.0f, -60.0f, 0.0f, 0.0 );
				// 	break;
				// case 2:
				// 	randomDirection = vec3( 0.0f, -60.0f, 50.0f, 0.0 );
				// 	break;
				// case 3:
				// 	randomDirection = vec3( 0.0f, -60.0f, -50.0f, 0.0 );
				// 	break;
				// }

				indices.push_back(0);
				indices.push_back(1);
				indices.push_back(2);
				indices.push_back(3);
				indices.push_back(0);
				indices.push_back(2);

				indices.push_back(4);
				indices.push_back(0);
				indices.push_back(3);
				indices.push_back(7);
				indices.push_back(4);
				indices.push_back(3);

				indices.push_back(4);
				indices.push_back(5);
				indices.push_back(1);
				indices.push_back(0);
				indices.push_back(4);
				indices.push_back(1);

				indices.push_back(1);
				indices.push_back(5);
				indices.push_back(6);
				indices.push_back(2);
				indices.push_back(1);
				indices.push_back(6);

				indices.push_back(5);
				indices.push_back(4);
				indices.push_back(7);
				indices.push_back(6);
				indices.push_back(5);
				indices.push_back(7);

				indices.push_back(3);
				indices.push_back(2);
				indices.push_back(6);
				indices.push_back(7);
				indices.push_back(3);
				indices.push_back(6);

				// std::cout << "x: " << half_x_rand << std::endl;
				// std::cout << "y: " << half_y_rand << std::endl;
				// std::cout << "z: " << half_z_rand << std::endl;

				// std::cout << "TEST " << -half_x_rand << std::endl;

				allMeshMaxAbsoluteValues.Push({});

				meshAxisLimitingValues.highest_x = -MAXFLOAT;
				meshAxisLimitingValues.lowest_x  = MAXFLOAT;
				meshAxisLimitingValues.highest_y = -MAXFLOAT;
				meshAxisLimitingValues.lowest_y  = MAXFLOAT;
				meshAxisLimitingValues.highest_z = -MAXFLOAT;
				meshAxisLimitingValues.lowest_z  = MAXFLOAT;
				
				unsigned int cube_vertices = 8;
				for ( unsigned int i = 0; i < cube_vertices; ++i ) {
					vec4 joinIndices;
					vec4 weights;
					joinIndices[0] = -1;
					joinIndices[1] = -1;
					joinIndices[2] = -1;
					joinIndices[3] = -1;

					weights[0] = 1;
					weights[1] = 1;
					weights[2] = 1;
					weights[3] = 1;

					SVertex vertex;
					float half_x = half_x_rand;
					float half_y = half_y_rand;
					float half_z = half_z_rand;
					// float max    = 0.5f;
					// if ( half_x_rand > half_z_rand ) {
					// 	half_x = max;
					// 	half_z = max * (float)half_z_rand / (float)half_x_rand;
					// } else {
					// 	half_z = max;
					// 	half_x = max * (float)half_x_rand / (float)half_z_rand;
					// }
					
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

					if ( vertex[0] < meshAxisLimitingValues.lowest_x ) {
						meshAxisLimitingValues.lowest_x = vertex[0];
					} else if ( vertex[0] > meshAxisLimitingValues.highest_x ) {
						meshAxisLimitingValues.highest_x = vertex[0];
					}

					if ( vertex[1] < meshAxisLimitingValues.lowest_y ) {
						meshAxisLimitingValues.lowest_y = vertex[1];
					} else if ( vertex[1] > meshAxisLimitingValues.highest_y ) {
						meshAxisLimitingValues.highest_y = vertex[1];
					}

					if ( vertex[2] < meshAxisLimitingValues.lowest_z ) {
						meshAxisLimitingValues.lowest_z = vertex[2];
					} else if ( vertex[2] > meshAxisLimitingValues.highest_z ) {
						meshAxisLimitingValues.highest_z = vertex[2];
					}
					
					// std::cout << "vertex x: " << vertex[0] << std::endl;
					// std::cout << "vertex y: " << vertex[1] << std::endl;
					// std::cout << "vertex z: " << vertex[2] << std::endl;
					
					SVertex normal;
					normal[0] = 0;
					normal[1] = 1;
					normal[2] = 0;
					SVertex texture;
					texture[0] = 0;
					texture[1] = 1;

					nextLevel.Push({{vertex[0], vertex[1], vertex[2]},
									{normal[0], normal[1], normal[2]},
									{texture[0], texture[1]},
									{joinIndices[0], joinIndices[1], joinIndices[2], joinIndices[3]},
									{weights[0], weights[1], weights[2], weights[3]}});
				}

				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x = (meshAxisLimitingValues.highest_x - meshAxisLimitingValues.lowest_x) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y = (meshAxisLimitingValues.highest_y - meshAxisLimitingValues.lowest_y) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z = (meshAxisLimitingValues.highest_z - meshAxisLimitingValues.lowest_z) / 2.0f;
				// std::cout << "size of all mesh container in proc gen 1: " << allMeshMaxAbsoluteValues.GetSize() << std::endl;
				// std::cout << "pocedural gen" << std::endl;
				// std::cout << "max width: " << meshAxisLimitingValues.highest_x << std::endl;
				// std::cout << "min width: " << meshAxisLimitingValues.lowest_x << std::endl;
				// std::cout << "max height: " << meshAxisLimitingValues.highest_y << std::endl;
				// std::cout << "min height: " << meshAxisLimitingValues.lowest_y << std::endl;
				// std::cout << "max deep: " << meshAxisLimitingValues.highest_z << std::endl;
				// std::cout << "min deep: " << meshAxisLimitingValues.lowest_z << std::endl;
			
				// std::cout << "half width: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x << std::endl;
				// std::cout << "half height: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y << std::endl;
				// std::cout << "half deep: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z << std::endl;
				
				[[maybe_unused]] cm::MeshHandle gameLevelMeshHandle = GLVM->LoadMesh();
//				std::cout << "proc gen mesh id: " << gameLevelMeshHandle.id << std::endl;
				Entity gameLevelEntity = EntityManager->CreateEntity();
				ComponentManager->CreateComponent<cm::material, cm::collider, cm::mesh, cm::transform, cm::actor>(gameLevelEntity);
//				std::cout << "current level position before set in component: " << currentLevelPosition << std::endl;
				*ComponentManager->GetComponent<cm::transform>(gameLevelEntity) = { .position = currentLevelPosition, .yaw = 0.0f, .pitch = 0.0f, .scale = 1.0f, .gltf = true };
				ComponentManager->GetComponent<cm::mesh>(gameLevelEntity)->handle = gameLevelMeshHandle;
				cm::material* gameLevelMaterial  = ComponentManager->GetComponent<cm::material>(gameLevelEntity);
				ecs::TextureHandle gameLevelTexture = textureHandlers[2];
				*gameLevelMaterial = { .diffuseTextureID_ = gameLevelTexture, .specularTextureID_ = gameLevelTexture, .ambient = { 0.05f, 0.05f, 0.0f },
					.shininess = 128.0f * 0.078125f };
//				std::cout << "TEST PROC" << std::endl;

				transitionBridgeIndices.push_back(0);
				transitionBridgeIndices.push_back(1);
				transitionBridgeIndices.push_back(2);
				transitionBridgeIndices.push_back(3);
				transitionBridgeIndices.push_back(0);
				transitionBridgeIndices.push_back(2);

				transitionBridgeIndices.push_back(4);
				transitionBridgeIndices.push_back(0);
				transitionBridgeIndices.push_back(3);
				transitionBridgeIndices.push_back(7);
				transitionBridgeIndices.push_back(4);
				transitionBridgeIndices.push_back(3);

				transitionBridgeIndices.push_back(4);
				transitionBridgeIndices.push_back(5);
				transitionBridgeIndices.push_back(1);
				transitionBridgeIndices.push_back(0);
				transitionBridgeIndices.push_back(4);
				transitionBridgeIndices.push_back(1);

				transitionBridgeIndices.push_back(1);
				transitionBridgeIndices.push_back(5);
				transitionBridgeIndices.push_back(6);
				transitionBridgeIndices.push_back(2);
				transitionBridgeIndices.push_back(1);
				transitionBridgeIndices.push_back(6);

				transitionBridgeIndices.push_back(5);
				transitionBridgeIndices.push_back(4);
				transitionBridgeIndices.push_back(7);
				transitionBridgeIndices.push_back(6);
				transitionBridgeIndices.push_back(5);
				transitionBridgeIndices.push_back(7);

				transitionBridgeIndices.push_back(3);
				transitionBridgeIndices.push_back(2);
				transitionBridgeIndices.push_back(6);
				transitionBridgeIndices.push_back(7);
				transitionBridgeIndices.push_back(3);
				transitionBridgeIndices.push_back(6);
				
				allMeshMaxAbsoluteValues.Push({});

				meshAxisLimitingValues.highest_x = -MAXFLOAT;
				meshAxisLimitingValues.lowest_x  = MAXFLOAT;
				meshAxisLimitingValues.highest_y = -MAXFLOAT;
				meshAxisLimitingValues.lowest_y  = MAXFLOAT;
				meshAxisLimitingValues.highest_z = -MAXFLOAT;
				meshAxisLimitingValues.lowest_z  = MAXFLOAT;
				
				for ( unsigned int i = 0; i < cube_vertices; ++i ) {
					vec4 joinIndices;
					vec4 weights;
					joinIndices[0] = -1;
					joinIndices[1] = -1;
					joinIndices[2] = -1;
					joinIndices[3] = -1;

					weights[0] = 1;
					weights[1] = 1;
					weights[2] = 1;
					weights[3] = 1;

					SVertex vertex;
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
					// float max    = 0.5f;
					// if ( half_x_rand > half_z_rand ) {
					// 	half_x = max;
					// 	half_z = max * (float)half_z_rand / (float)half_x_rand;
					// } else {
					// 	half_z = max;
					// 	half_x = max * (float)half_x_rand / (float)half_z_rand;
					// }
					
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

					if ( vertex[0] < meshAxisLimitingValues.lowest_x ) {
						meshAxisLimitingValues.lowest_x = vertex[0];
					} else if ( vertex[0] > meshAxisLimitingValues.highest_x ) {
						meshAxisLimitingValues.highest_x = vertex[0];
					}

					if ( vertex[1] < meshAxisLimitingValues.lowest_y ) {
						meshAxisLimitingValues.lowest_y = vertex[1];
					} else if ( vertex[1] > meshAxisLimitingValues.highest_y ) {
						meshAxisLimitingValues.highest_y = vertex[1];
					}

					if ( vertex[2] < meshAxisLimitingValues.lowest_z ) {
						meshAxisLimitingValues.lowest_z = vertex[2];
					} else if ( vertex[2] > meshAxisLimitingValues.highest_z ) {
						meshAxisLimitingValues.highest_z = vertex[2];
					}
					
					// std::cout << "vertex x: " << vertex[0] << std::endl;
					// std::cout << "vertex y: " << vertex[1] << std::endl;
					// std::cout << "vertex z: " << vertex[2] << std::endl;
					
					SVertex normal;
					normal[0] = 0;
					normal[1] = 1;
					normal[2] = 0;
					SVertex texture;
					texture[0] = 0;
					texture[1] = 1;

					transitionBridgeVertices.Push({{vertex[0], vertex[1], vertex[2]},
									{normal[0], normal[1], normal[2]},
									{texture[0], texture[1]},
									{joinIndices[0], joinIndices[1], joinIndices[2], joinIndices[3]},
									{weights[0], weights[1], weights[2], weights[3]}});
				}

				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x = (meshAxisLimitingValues.highest_x - meshAxisLimitingValues.lowest_x) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y = (meshAxisLimitingValues.highest_y - meshAxisLimitingValues.lowest_y) / 2.0f;
				allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z = (meshAxisLimitingValues.highest_z - meshAxisLimitingValues.lowest_z) / 2.0f;
				// std::cout << "size of all mesh container in proc gen 1: " << allMeshMaxAbsoluteValues.GetSize() << std::endl;
				// std::cout << "pocedural gen" << std::endl;
				// std::cout << "max width: " << meshAxisLimitingValues.highest_x << std::endl;
				// std::cout << "min width: " << meshAxisLimitingValues.lowest_x << std::endl;
				// std::cout << "max height: " << meshAxisLimitingValues.highest_y << std::endl;
				// std::cout << "min height: " << meshAxisLimitingValues.lowest_y << std::endl;
				// std::cout << "max deep: " << meshAxisLimitingValues.highest_z << std::endl;
				// std::cout << "min deep: " << meshAxisLimitingValues.lowest_z << std::endl;
			
				// std::cout << "half width: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x << std::endl;
				// std::cout << "half height: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y << std::endl;
				// std::cout << "half deep: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z << std::endl;
				
				[[maybe_unused]] cm::MeshHandle transitionBridgeMeshHandle = GLVM->LoadMesh();
//				std::cout << "proc gen mesh id: " << gameLevelMeshHandle.id << std::endl;
				Entity transitionBridgeEntity = EntityManager->CreateEntity();
				ComponentManager->CreateComponent<cm::material, cm::collider, cm::mesh, cm::transform, cm::actor>(transitionBridgeEntity);
				*ComponentManager->GetComponent<cm::transform>(transitionBridgeEntity) = { .position = transitionBridgePosition, .yaw = 0.0f, .pitch = 0.0f, .scale = 1.0f, .gltf = true };
				ComponentManager->GetComponent<cm::mesh>(transitionBridgeEntity)->handle = transitionBridgeMeshHandle;
				cm::material* transitionBridgeMaterial  = ComponentManager->GetComponent<cm::material>(transitionBridgeEntity);
				ecs::TextureHandle transitionBridgeTexture = textureHandlers[3];
				*transitionBridgeMaterial = { .diffuseTextureID_ = transitionBridgeTexture, .specularTextureID_ = transitionBridgeTexture, .ambient = { 0.05f, 0.05f, 0.0f },
					.shininess = 128.0f * 0.078125f };
//				std::cout << "TEST PROC" << std::endl;
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
				previousIterationTransitionBridgeDirection = (4 + previousIterationTransitionBridgeDirection) % 4 + 1;
			} else {
				setCoordinateMaximumValuePerDirection(currentLevelPosition, (float)half_x_rand, (float)half_y_rand, (float)half_z_rand);
				validLevel = true;
			}
		}
		currentLevelPosition[1] = 0.0f;
	}

	void ProceduralLevelGeneratingSystem::generateTransitionBridge( const unsigned int half_x_rand, const unsigned int half_y_rand, const unsigned int half_z_rand,
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
			if ( nextLevelTransitionDirection == 1 || nextLevelTransitionDirection == 3 ) {
				std::uniform_int_distribution<int> distTransitionBridgeAnchorPoint( 0, half_x_rand * 2 - 1 );    ///< In what point we connect next transition bridge to current level
				transitionBridgeAnchorPoint = distTransitionBridgeAnchorPoint(mersenne);
				transitionBridgeOffset_x = -(float)half_x_rand + (float)transitionBridgeAnchorPoint;
				if ( nextLevelTransitionDirection == 1 ) {
					transitionBridgeOffset_z = half_z_rand;
					transitionBridgePosition = { currentLevelPosition[0] + transitionBridgeOffset_x + transitionBridgeHalfWidth, (float)half_y_rand,
						currentLevelPosition[2] + transitionBridgeOffset_z + transitionBridgeHalfHeight };
				} else {
					transitionBridgeOffset_z = -(float)half_z_rand;
					transitionBridgePosition = { currentLevelPosition[0] + transitionBridgeOffset_x + transitionBridgeHalfWidth, (float)half_y_rand,
						currentLevelPosition[2] + transitionBridgeOffset_z - transitionBridgeHalfHeight };
				}
			} else if ( nextLevelTransitionDirection == 2 || nextLevelTransitionDirection == 4 ) {
				std::uniform_int_distribution<int> distTransitionBridgeAnchorPoint( 0, half_z_rand * 2 - 1 );    ///< In what point we connect next transition bridge to current level
				transitionBridgeAnchorPoint = distTransitionBridgeAnchorPoint(mersenne);
				transitionBridgeOffset_z    = -(float)half_z_rand + (float)transitionBridgeAnchorPoint;
				if ( nextLevelTransitionDirection == 2 ) {
					transitionBridgeOffset_x = half_x_rand;
					transitionBridgePosition = { currentLevelPosition[0] + transitionBridgeOffset_x + transitionBridgeHalfHeight, (float)half_y_rand,
						currentLevelPosition[2] + transitionBridgeOffset_z + transitionBridgeHalfWidth };
				} else {
					transitionBridgeOffset_x = -(float)half_x_rand;
					transitionBridgePosition = { currentLevelPosition[0] + transitionBridgeOffset_x - transitionBridgeHalfHeight, (float)half_y_rand,
						currentLevelPosition[2] + transitionBridgeOffset_z + transitionBridgeHalfWidth };
				}
			}

			float width = 0;
			float height = 0;
			if ( nextLevelTransitionDirection == 1 || nextLevelTransitionDirection == 3 ) {
				width = transitionBridgeHalfWidth;
				height = transitionBridgeHalfHeight;
			}
			if ( nextLevelTransitionDirection == 2 || nextLevelTransitionDirection == 4 ) {
				width = transitionBridgeHalfHeight;
				height = transitionBridgeHalfWidth;
			}
					
			if ( checkCollisionIntersectionWithMaximumCoordinates(transitionBridgePosition, width, half_y_rand, height ) ) {
				nextLevelTransitionDirection = (4 + nextLevelTransitionDirection) % 4 + 1;
			} else {
				setCoordinateMaximumValuePerDirection(transitionBridgePosition, (float)width, (float)half_y_rand, (float)height);
				validTransitionBridge = true;
			}
		}
		transitionBridgePosition[1]         = currentLevelPosition[1];
		previousIterationTransitionBridgeDirection = nextLevelTransitionDirection;
	}
	
	void ProceduralLevelGeneratingSystem::setCoordinateMaximumValuePerDirection(vec3 position, float half_x, float half_y, float half_z) {
		if ( position[0] + half_x > coordinateMaximumValuePerDirection.positive_x ) {
			coordinateMaximumValuePerDirection.positive_x = position[0] + half_x;
		}
		if ( position[0] - half_x < coordinateMaximumValuePerDirection.negative_x ) {
			coordinateMaximumValuePerDirection.negative_x = position[0] - half_x;
		}
		if ( position[1] + half_y > coordinateMaximumValuePerDirection.positive_y ) {
			coordinateMaximumValuePerDirection.positive_y = position[1] + half_y;
		}
		if ( position[1] - half_y < coordinateMaximumValuePerDirection.negative_y ) {
			coordinateMaximumValuePerDirection.negative_y = position[1] - half_y;
		}
		if ( position[2] + half_z > coordinateMaximumValuePerDirection.positive_z ) {
			coordinateMaximumValuePerDirection.positive_z = position[2] + half_z;
		}
		if ( position[2] - half_z < coordinateMaximumValuePerDirection.negative_z ) {
			coordinateMaximumValuePerDirection.negative_z = position[2] - half_z;
		}
	}

	bool ProceduralLevelGeneratingSystem::checkCollisionIntersectionWithMaximumCoordinates(vec3 position, float half_x, float half_y, float half_z) {
		if ( position[0] + half_x > coordinateMaximumValuePerDirection.negative_x &&
			 position[0] - half_x < coordinateMaximumValuePerDirection.positive_x &&
			 position[1] + half_y > coordinateMaximumValuePerDirection.negative_y &&
			 position[1] - half_y < coordinateMaximumValuePerDirection.positive_y &&
			 position[2] + half_z > coordinateMaximumValuePerDirection.negative_z &&
			 position[2] - half_z < coordinateMaximumValuePerDirection.positive_z ) {
			return true;
		} else {
			return false;
		}
	}
}
