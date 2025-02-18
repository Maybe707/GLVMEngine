#include <ProceduralLevelGeneratingSystem.hpp>
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

		if ( bredoFlag == false ) {
			core::vector<core::Vertex> nextLevel;
			std::vector<uint32_t> indices;

			if ( levelNubmer < 1 ) {
				std::random_device rd;
				std::map<int, int> hist;
				std::mt19937 mersenne(rd());
				std::uniform_int_distribution<int> dist(1, 3);
				// unsigned int half_x = dist(mersenne);
				// unsigned int half_y = dist(mersenne);
				// unsigned int half_z = dist(mersenne);

				unsigned int half_x = 1;
				unsigned int half_y = 2;
				unsigned int half_z = 3;
				
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

				std::cout << "x: " << half_x << std::endl;
				std::cout << "y: " << half_y << std::endl;
				std::cout << "z: " << half_z << std::endl;

				std::cout << "TEST " << -half_x << std::endl;
				
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
					float devider = 10.0f;
					switch( i ) {
					case 0:
						vertex[0] = half_x / devider;
						vertex[1] = half_y / devider;
						vertex[2] = half_z / devider;
						break;
					case 1:
						vertex[0] = -(float)half_x / devider;
						vertex[1] = half_y / devider;
						vertex[2] = half_z / devider;
						break;			
					case 2:
						vertex[0] = -(float)half_x / devider;
						vertex[1] = -(float)half_y / devider;
						vertex[2] = half_z / devider;
						break;			
					case 3:
						vertex[0] = half_x / devider;
						vertex[1] = -(float)half_y / devider;
						vertex[2] = half_z / devider;
						break;
					case 4:
						vertex[0] = half_x / devider;
						vertex[1] = half_y / devider;
						vertex[2] = -(float)half_z / devider;
						break;
					case 5:
						vertex[0] = -(float)half_x / devider;
						vertex[1] = half_y / devider;
						vertex[2] = -(float)half_z / devider;
						break;			
					case 6:
						vertex[0] = -(float)half_x / devider;
						vertex[1] = -(float)half_y / devider;
						vertex[2] = -(float)half_z / devider;
						break;			
					case 7:
						vertex[0] = half_x / devider;
						vertex[1] = -(float)half_y / devider;
						vertex[2] = -(float)half_z / devider;
						break;			
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
								
				[[maybe_unused]] cm::MeshHandle gameLevelMeshHandle = GLVM->LoadMesh();
				Entity plain0 = EntityManager->CreateEntity();
				ComponentManager->CreateComponent<cm::material, cm::collider, cm::mesh, cm::transform, cm::actor>(plain0);
				*ComponentManager->GetComponent<cm::transform>(plain0) = { .position = { 0.0, 5.0, 15.0 }, .yaw = 0.0f, .pitch = 0.0f, .scale = 1.0f, .gltf = false };
				ComponentManager->GetComponent<cm::mesh>(plain0)->handle = gameLevelMeshHandle;
				cm::material* materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
				ecs::TextureHandle grayTextureHandle = textureHandlers[3];
				*materialPlain0 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle, .ambient = { 0.05f, 0.05f, 0.0f },
					.shininess = 128.0f * 0.078125f };
			
				++levelNubmer;
			}
			levelGeneratedVertices.push_back(nextLevel);
			levelGeneratedIndices.push_back(indices);
			
			bredoFlag = true;
		}
	}
}
