#include <ProceduralLevelGeneratingSystem.hpp>
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
				std::uniform_int_distribution<int> dist(0, 3);

				unsigned int random = dist(mersenne);
				vec3 randomDirection = {};
				switch( random ) {
				case 0:
					randomDirection = vec3( 50.0f, -60.0f, 0.0f, 0.0 );
					break;
				case 1:
					randomDirection = vec3( -50.0f, -60.0f, 0.0f, 0.0 );
					break;
				case 2:
					randomDirection = vec3( 0.0f, -60.0f, 50.0f, 0.0 );
					break;
				case 3:
					randomDirection = vec3( 0.0f, -60.0f, -50.0f, 0.0 );
					break;
				}

				indices.push_back(0);
				indices.push_back(1);
				indices.push_back(2);
				indices.push_back(3);
				indices.push_back(0);
				indices.push_back(2);
				
				for ( unsigned int i = 0; i < 4; ++i ) {
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
					switch( i ) {
					case 0:
						vertex[0] = 0.1;
						vertex[1] = 0.1;
						break;
					case 1:
						vertex[0] = -0.1;
						vertex[1] = 0.1;
						break;			
					case 2:
						vertex[0] = -0.1;
						vertex[1] = -0.1;
						break;			
					case 3:
						vertex[0] = 0.1;
						vertex[1] = -0.1;
						break;			
					}
					vertex[2] = -0.1;
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
				ComponentManager->CreateComponent<cm::material, cm::mesh, cm::transform, cm::actor>(plain0);
				*ComponentManager->GetComponent<cm::transform>(plain0) = { .position = { 0.5, 5.0, 20.0 }, .yaw = 0.0f, .pitch = 0.0f, .scale = 40.2f, .gltf = true };
				ComponentManager->GetComponent<cm::mesh>(plain0)->handle = gameLevelMeshHandle;
				cm::material* materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
				ecs::TextureHandle grayTextureHandle = textureHandlers[2];
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
