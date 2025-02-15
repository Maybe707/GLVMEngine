#include <ProceduralLevelGeneratingSystem.hpp>
#include "Components/VertexComponent.hpp"
#include "Engine.hpp"

namespace GLVM::core
{
	void ProceduralLevelGeneratingSystem::Update() {
		using namespace GLVM;
		namespace cm  = GLVM::ecs::components;
		core::Engine* GLVM = core::Engine::GetInstance();
		ecs::EntityManager   * EntityManager     = ecs::EntityManager::GetInstance();
		ecs::ComponentManager* ComponentManager  = ecs::ComponentManager::GetInstance();

		if ( bredoFlag == false ) { 
			if ( levelNubmer < 3 ) {
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
			
				// Entity plain0 = EntityManager->CreateEntity();
				// ComponentManager->CreateComponent<cm::material, cm::mesh, cm::transform, cm::collider, cm::actor>(plain0);
				// *ComponentManager->GetComponent<cm::transform>(plain0) = { .position = randomDirection, .yaw = 0.0f, .pitch = 0.0f, .scale = 40.2f, .gltf = true };
				// ecs::components::MeshHandle cubeHandle_OBJ = meshHandlers[0];
				// ComponentManager->GetComponent<cm::mesh>(plain0)->handle = cubeHandle_OBJ;
				// cm::material* materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
				// ecs::TextureHandle grayTextureHandle = textureHandlers[2];
				// *materialPlain0 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle, .ambient = { 0.05f, 0.05f, 0.0f },
				// 	.shininess = 128.0f * 0.078125f };

				[[maybe_unused]] cm::MeshHandle gameLevelMeshHandle = GLVM->LoadMesh();
				Entity plain0 = EntityManager->CreateEntity();
				ComponentManager->CreateComponent<cm::material, cm::mesh, cm::transform, cm::actor>(plain0);
				*ComponentManager->GetComponent<cm::transform>(plain0) = { .position = { 0.5, 0.5, 30.0 }, .yaw = 0.0f, .pitch = 0.0f, .scale = 40.2f, .gltf = true };
				ComponentManager->GetComponent<cm::mesh>(plain0)->handle = gameLevelMeshHandle;
				cm::material* materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
				ecs::TextureHandle grayTextureHandle = textureHandlers[2];
				*materialPlain0 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle, .ambient = { 0.05f, 0.05f, 0.0f },
					.shininess = 128.0f * 0.078125f };
			
				++levelNubmer;
			}
			bredoFlag = true;
		}
	}
}
