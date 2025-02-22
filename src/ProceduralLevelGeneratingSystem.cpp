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

		while ( levelNubmer < 3 ) {
			core::vector<core::Vertex> nextLevel;
			std::vector<uint32_t> indices;
			std::cout << "size of all mesh container in proc gen 0: " << allMeshMaxAbsoluteValues.GetSize() << std::endl;
			allMeshMaxAbsoluteValues.Push({});

			meshAxisLimitingValues.highest_x = -MAXFLOAT;
			meshAxisLimitingValues.lowest_x  = MAXFLOAT;
			meshAxisLimitingValues.highest_y = -MAXFLOAT;
			meshAxisLimitingValues.lowest_y  = MAXFLOAT;
			meshAxisLimitingValues.highest_z = -MAXFLOAT;
			meshAxisLimitingValues.lowest_z  = MAXFLOAT;
			if ( levelNubmer < 3 ) {
				std::random_device rd;
				std::map<int, int> hist;
				std::mt19937 mersenne(rd());
				std::uniform_int_distribution<int> dist(1, 3);
				unsigned int half_x_rand = dist(mersenne);
				unsigned int half_y_rand = dist(mersenne);
				unsigned int half_z_rand = dist(mersenne);

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

				std::cout << "x: " << half_x_rand << std::endl;
				std::cout << "y: " << half_y_rand << std::endl;
				std::cout << "z: " << half_z_rand << std::endl;

				std::cout << "TEST " << -half_x_rand << std::endl;
				
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
					
					std::cout << "vertex x: " << vertex[0] << std::endl;
					std::cout << "vertex y: " << vertex[1] << std::endl;
					std::cout << "vertex z: " << vertex[2] << std::endl;
					
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
				std::cout << "size of all mesh container in proc gen 1: " << allMeshMaxAbsoluteValues.GetSize() << std::endl;
				std::cout << "pocedural gen" << std::endl;
				std::cout << "max width: " << meshAxisLimitingValues.highest_x << std::endl;
				std::cout << "min width: " << meshAxisLimitingValues.lowest_x << std::endl;
				std::cout << "max height: " << meshAxisLimitingValues.highest_y << std::endl;
				std::cout << "min height: " << meshAxisLimitingValues.lowest_y << std::endl;
				std::cout << "max deep: " << meshAxisLimitingValues.highest_z << std::endl;
				std::cout << "min deep: " << meshAxisLimitingValues.lowest_z << std::endl;
			
				std::cout << "half width: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_x << std::endl;
				std::cout << "half height: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_y << std::endl;
				std::cout << "half deep: " << allMeshMaxAbsoluteValues[allMeshMaxAbsoluteValues.GetSize() - 1].absolute_z << std::endl;
				
				[[maybe_unused]] cm::MeshHandle gameLevelMeshHandle = GLVM->LoadMesh();
				std::cout << "proc gen mesh id: " << gameLevelMeshHandle.id << std::endl;
				Entity plain0 = EntityManager->CreateEntity();
				ComponentManager->CreateComponent<cm::material, cm::collider, cm::mesh, cm::transform, cm::actor>(plain0);
				*ComponentManager->GetComponent<cm::transform>(plain0) = { .position = { (float)levelNubmer * 7, 1.0, 15.0 }, .yaw = 0.0f, .pitch = 0.0f, .scale = 1.0f, .gltf = true };
				ComponentManager->GetComponent<cm::mesh>(plain0)->handle = gameLevelMeshHandle;
				cm::material* materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
				ecs::TextureHandle grayTextureHandle = textureHandlers[3];
				*materialPlain0 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle, .ambient = { 0.05f, 0.05f, 0.0f },
					.shininess = 128.0f * 0.078125f };
				std::cout << "TEST PROC" << std::endl;
				++levelNubmer;
			}
			levelGeneratedVertices.push_back(nextLevel);
			levelGeneratedIndices.push_back(indices);
			
			bredoFlag = true;
		}
	}
}
