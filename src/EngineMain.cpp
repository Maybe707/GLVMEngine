// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Components/AnimationMoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/FontComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InterfaceComponent.hpp"
#include "Components/InterfaceComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/InventorySlotComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/ActorComponent.hpp"
#include "Engine.hpp"
#include "Network/UDP_ServerLinux.hpp"
#include "SpritesData.hpp"
#include "Texture.hpp"
#include "VertexMath.hpp"

#include <cstdio>
#include <map>
#include <random>

int main()
{
	using namespace GLVM;
	namespace cm  = GLVM::ecs::components;

	
	ecs::EntityManager   * EntityManager     = ecs::EntityManager::GetInstance();
	ecs::ComponentManager* ComponentManager  = ecs::ComponentManager::GetInstance();

	core::Engine* GLVM = core::Engine::GetInstance();
	[[maybe_unused]] cm::MeshHandle cubeHandle_OBJ = GLVM->LoadMeshFromFile_OBJ("../waveFrontObj/cube.obj");
	[[maybe_unused]] cm::MeshHandle coneHandle_OBJ = GLVM->LoadMeshFromFile_OBJ("../waveFrontObj/cone.obj");
	[[maybe_unused]] cm::MeshHandle icoSphereHandle_OBJ = GLVM->LoadMeshFromFile_OBJ("../waveFrontObj/ico_sphere.obj");
	[[maybe_unused]] cm::MeshHandle monkeyHandle_OBJ = GLVM->LoadMeshFromFile_OBJ("../waveFrontObj/suzana.obj");
//	[[maybe_unused]] cm::MeshHandle monkeyHandle_OBJ = GLVM->LoadMeshFromFile_OBJ("../waveFrontObj/suzana2.obj");
	[[maybe_unused]] cm::MeshHandle uvSphereHandle_OBJ = GLVM->LoadMeshFromFile_OBJ("../waveFrontObj/uv_sphere.obj");
	[[maybe_unused]] cm::MeshHandle torusHandle_OBJ = GLVM->LoadMeshFromFile_OBJ("../waveFrontObj/torus.obj");
	[[maybe_unused]] cm::MeshHandle pipeHandle_OBJ = GLVM->LoadMeshFromFile_OBJ("../waveFrontObj/pipe.obj");
	[[maybe_unused]] cm::MeshHandle hyperCubeHandle_GLTF = GLVM->LoadMeshFromFile_GLTF("../gltf/hyper_cube.gltf");
	[[maybe_unused]] cm::MeshHandle hyperCubeHandle2_GLTF = GLVM->LoadMeshFromFile_GLTF("../gltf/hyper_cube2.gltf");
	[[maybe_unused]] cm::MeshHandle megaChelHandle_GLTF = GLVM->LoadMeshFromFile_GLTF("../gltf/mega_chel.gltf");
	[[maybe_unused]] cm::MeshHandle simpleCubeHandle_GLTF = GLVM->LoadMeshFromFile_GLTF("../gltf/simpleCube2.gltf");
	[[maybe_unused]] cm::MeshHandle crosshair_001_Handle_GLTF = GLVM->LoadMeshFromFile_GLTF("../gltf/crosshair_001.gltf");
	[[maybe_unused]] cm::MeshHandle inventory_Handle_GLTF = GLVM->LoadMeshFromFile_GLTF("../gltf/inventory.gltf");

	[[maybe_unused]] ecs::TextureHandle chelikTextureHandle = GLVM->LoadTextureFromAddress(128, 96, chelik_dat_len, chelik_dat);
	[[maybe_unused]] ecs::TextureHandle witchTexturehandle = GLVM->LoadTextureFromAddress(32, 32, witch_dat_len, witch_dat);
	[[maybe_unused]] ecs::TextureHandle grayTextureHandle = GLVM->LoadTextureFromAddress(32, 32, gray_dat_len, gray_dat);
	[[maybe_unused]] ecs::TextureHandle container2Texturehandle = GLVM->LoadTextureFromAddress(500, 500, container2_dat_len, container2_dat);
	[[maybe_unused]] ecs::TextureHandle container2SpecularTextureHandle = GLVM->LoadTextureFromAddress(500, 500, container2_specular_dat_len, container2_specular_dat);
	[[maybe_unused]] ecs::TextureHandle crosshairTexturehandle = GLVM->LoadTextureFromAddress(32, 32, Crosshair_dat_len, Crosshair_dat);
	[[maybe_unused]] ecs::TextureHandle fontAtlasTexturehandle = GLVM->LoadTextureFromAddress(84, 132, fontAtlas_dat_len, fontAtlas_dat);
	[[maybe_unused]] ecs::TextureHandle inventoryTexturehandle = GLVM->LoadTextureFromAddress(64, 64, inventorySlot_dat_len, inventorySlot_dat);
	[[maybe_unused]] ecs::TextureHandle tilesetTexturehandle = GLVM->LoadTextureFromAddress(512, 512, tileset_dat_len, tileset_dat);

	/// Loading method with stb_image
	// [[maybe_unused]] ecs::TextureHandle chelikTextureHandle = GLVM->LoadTextureFromFile("../textures/chelik.h");
	// [[maybe_unused]] ecs::TextureHandle witchTexturehandle = GLVM->LoadTextureFromFile("../textures/witch.h");
	// [[maybe_unused]] ecs::TextureHandle grayTextureHandle = GLVM->LoadTextureFromFile("../textures/data/gray.png");
	// [[maybe_unused]] ecs::TextureHandle container2Texturehandle = GLVM->LoadTextureFromFile("../textures/data/container2.png");
	// [[maybe_unused]] ecs::TextureHandle container2SpecularTextureHandle = GLVM->LoadTextureFromFile("../textures/data/container2_specular.png");

	// while(1) {
	// 	core::UDP_ServerLinux serverLinux;
	// 	char* buffer = serverLinux.receive();
	// 	printf("Message from client: %s\n", buffer);
	// 	// for ( int i = 0; i < 1024; ++i ) {
	// 	// 	if ( buffer[i] == '\n' )
	// 	// 		break;

			
	// 	// }

	// 	serverLinux.response();
	// }
	
    Entity uiPlayer = EntityManager->CreateEntity();
    ComponentManager->CreateComponent<cm::mesh, cm::controller, cm::collider, cm::beholder,
		cm::transform, cm::rigidBody, cm::health, cm::actor, cm::material>(uiPlayer);
	*ComponentManager->GetComponent<cm::transform>(uiPlayer) = { .position = { 5.0f, 10.0f, 15.0f }, .scale = 1.0f, .gltf = true };
	*ComponentManager->GetComponent<cm::rigidBody>(uiPlayer) = { .fMass_ = 6.0f };
	*ComponentManager->GetComponent<cm::health>(uiPlayer) = { .maxHealth = 100, .currentHealth = 100 };
    *ComponentManager->GetComponent<cm::beholder>(uiPlayer) = { .forward = { 0.0f, 0.0f, -1.0f },
		.up = { 0.0f, -1.0f, 0.0f }, .Position = {0.0f, 0.0f, -3.0f} };
    ComponentManager->GetComponent<cm::mesh>(uiPlayer)->handle = monkeyHandle_OBJ;
	cm::material* materialPlainPlayer  = ComponentManager->GetComponent<cm::material>(uiPlayer);
	*materialPlainPlayer = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };

	// Entity plain0 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::material, cm::mesh, cm::transform, cm::collider, cm::actor>(plain0);
	// *ComponentManager->GetComponent<cm::transform>(plain0) = { .position = { 0.0, -20.0, 0 }, .yaw = 0.0f, .pitch = 0.0f, .scale = 20.0f, .gltf = true };
	// ComponentManager->GetComponent<cm::mesh>(plain0)->handle = hyperCubeHandle_GLTF;
	// cm::material* materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
	// *materialPlain0 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle, .ambient = { 0.05f, 0.05f, 0.0f },
	// 	.shininess = 128.0f * 0.078125f };


	int* arrayPtr = new int[10];
	for( int i = 0; i < 10; ++i ) {
		arrayPtr[i] = i * 2;
		std::cout << arrayPtr[i] << std::endl;
	}

	int array[3][2] = { { 1, 2 }, { 20, 30 }, { 450, 665 } };
//	asm volatile ("" : : "r,m"(array) : "memory");
//	int array[3][2];
	array[0][1] = 1000;
	array[2][1] = 2500;
	for( int i = 0; i < 3; ++i )
		for( int j = 0; j < 2; ++j )
			std::cout << array[i][j] << std::endl;
	
    std::random_device rd;
    std::map<int, int> hist;
	std::mt19937 mersenne(rd());
    std::uniform_int_distribution<int> dist(0, 3);
	
	for ( u32 i = 0; i < 13; ++i ) {
	Entity uiWitch = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::font, cm::animation, cm::material, cm::mesh, cm::collider, cm::transform, cm::health, cm::enemy, cm::rigidBody, cm::state, cm::actor>(uiWitch);
	unsigned int random = dist(mersenne);
	vec3 randomDirection = {};
	switch( random ) {
	case 0:
		randomDirection = vec3( 3.0f, 0.0f, 0.0f, 0.0 );
		break;
	case 1:
		randomDirection = vec3( -3.0f, 0.0f, 0.0f, 0.0 );
		break;
	case 2:
		randomDirection = vec3( 0.0f, 0.0f, 3.0f, 0.0 );
		break;
	case 3:
		randomDirection = vec3( 0.0f, 0.0f, -3.0f, 0.0 );
		break;
	}
	*ComponentManager->GetComponent<cm::transform>(uiWitch) = { .position = { vec3( (float)i * 5, 5.0f, 0.0f ) + randomDirection },
		.yaw = 0.0f, .pitch = 0.0f, .scale = 1.2f, .gltf = false };
	*ComponentManager->GetComponent<cm::state>(uiWitch) = { .state = core::States::ROAMING };
	*ComponentManager->GetComponent<cm::rigidBody>(uiWitch) = { .fMass_ = 6.0f };
	*ComponentManager->GetComponent<cm::enemy>(uiWitch) = { .detectRadius = 10.0f };
	*ComponentManager->GetComponent<cm::health>(uiWitch) = { .maxHealth = 100, .currentHealth = 100 };
	cm::font* fontComponentWitch = ComponentManager->GetComponent<cm::font>(uiWitch);
	if ( i < 3 ) {
		fontComponentWitch->font_string.Push('1');
	} else if ( i < 6 ) {
		fontComponentWitch->font_string.Push('1');
		fontComponentWitch->font_string.Push('0');
	} else if ( i < 10 ) {
		fontComponentWitch->font_string.Push('1');
		fontComponentWitch->font_string.Push('0');
		fontComponentWitch->font_string.Push('E');
	} else {
		fontComponentWitch->font_string.Push('J');
		fontComponentWitch->font_string.Push('r');
	}
	// fontComponentWitch->font_string.Push('N');
	// fontComponentWitch->font_string.Push('K');
	fontComponentWitch->lifeTime = 0.0f;
	fontComponentWitch->removeble = false;
	ComponentManager->GetComponent<cm::mesh>(uiWitch)->handle = megaChelHandle_GLTF;
	cm::material* materialWitch  = ComponentManager->GetComponent<cm::material>(uiWitch);
	*materialWitch  = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2SpecularTextureHandle, .ambient = { 0.05f, 0.05f, 0.05f },
		.shininess = 128.0f * 0.078125f };
	}

 	Entity cube0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::font, cm::material, cm::mesh, cm::collider, cm::transform, cm::actor>(cube0);
	*ComponentManager->GetComponent<cm::transform>(cube0) = { .position = { 7.0f, 3.0f, 0.0f },
		.yaw = 0.0f, .pitch = 0.0f, .scale = 4.0f, .gltf = true };
    ComponentManager->GetComponent<cm::mesh>(cube0)->handle = hyperCubeHandle2_GLTF;
	// cm::font* fontComponentCube0 = ComponentManager->GetComponent<cm::font>(cube0);
	// fontComponentCube0->font_string.Push('1');
	// fontComponentCube0->font_string.Push('2');
	// fontComponentCube0->font_string.Push('3');
	// fontComponentCube0->font_string.Push('4');
	// fontComponentCube0->font_string.Push('5');
	// fontComponentCube0->lifeTime = -10.0f;
	cm::material* materialCube0  = ComponentManager->GetComponent<cm::material>(cube0);
	*materialCube0  = { .diffuseTextureID_ = tilesetTexturehandle, .specularTextureID_ = container2SpecularTextureHandle, .ambient = { 0.05f, 0.05f, 0.05f },
		.shininess = 128.0f * 0.078125f };

	Entity crosshair = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::crosshair, cm::transform>(crosshair);
	*ComponentManager->GetComponent<cm::transform>(crosshair) = { .scale = 0.01f };
	ComponentManager->GetComponent<cm::mesh>(crosshair)->handle = crosshair_001_Handle_GLTF;
	cm::material* materialCorsshair = ComponentManager->GetComponent<cm::material>(crosshair);
	*materialCorsshair = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2SpecularTextureHandle,
		.ambient = { 0.05f, 0.05f, 0.05f }, .shininess = 128.0f * 0.078125f };

	Entity inventory = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::transform, cm::inventory>(inventory);
	cm::inventory* inventoryComponent = ComponentManager->GetComponent<cm::inventory>(inventory);
	inventoryComponent->entityOwner = uiPlayer;
	for ( unsigned int i = 0; i < 8; ++i )
		for ( unsigned int j = 0; j < 8; ++j ) {
			inventoryComponent->slots[i][j] = EntityManager->CreateEntity();
//			std::cout << "inventoryComponent enetities: " << inventoryComponent->slots[i][j] << std::endl;
			ComponentManager->CreateComponent<cm::mesh, cm::inventorySlot, cm::transform, cm::collider>(inventoryComponent->slots[i][j]);
			*ComponentManager->GetComponent<cm::transform>(inventoryComponent->slots[i][j]) = { .scale = 0.05f };
			ComponentManager->GetComponent<cm::mesh>(inventoryComponent->slots[i][j])->handle = inventory_Handle_GLTF;
		}
	*ComponentManager->GetComponent<cm::transform>(inventory) = { .position = { -1.5f, 1.5f, 0.0f },
		.yaw = 0.0f, .pitch = 0.0f, .scale = 4.0f, .gltf = true };

	for ( unsigned int i = 0; i < 5; ++i ) {
		Entity testItem = EntityManager->CreateEntity();
		ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform, cm::item, cm::rigidBody, cm::actor>(testItem);
		[[maybe_unused]] unsigned int row = i + 1;
		ComponentManager->GetComponent<cm::item>(testItem)->itemSlotType = { row, 5 };
		*ComponentManager->GetComponent<cm::transform>(testItem) = { .position = { 3.0f, 15.0f, 10.0f + i * 2.0f },
			.yaw = 0.0f, .pitch = 0.0f, .scale = 0.05f, .gltf = true };
		*ComponentManager->GetComponent<cm::rigidBody>(testItem) = { .fMass_ = 2.0f };
		if ( i % 2 == 0 ) 
			ComponentManager->GetComponent<cm::mesh>(testItem)->handle = hyperCubeHandle_GLTF;
		else
			ComponentManager->GetComponent<cm::mesh>(testItem)->handle = hyperCubeHandle_GLTF;
		
		cm::material* materialTestItem  = ComponentManager->GetComponent<cm::material>(testItem);
		*materialTestItem  = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2SpecularTextureHandle, .ambient = { 0.05f, 0.05f, 0.05f },
			.shininess = 128.0f * 0.078125f };
	}

 	// Entity testItem2 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform, cm::item, cm::rigidBody>(testItem2);
	// *ComponentManager->GetComponent<cm::transform>(testItem2) = { .tPosition = { 3.0f, 15.0f, 13.0f },
	// 	.yaw = 0.0f, .pitch = 0.0f, .fScale = 2.0f, .gltf = false };
	// *ComponentManager->GetComponent<cm::rigidBody>(testItem2) = { .fMass_ = 2.0f };
    // ComponentManager->GetComponent<cm::mesh>(testItem2)->handle = icoSphereHandle_OBJ;
	// cm::material* materialTestItem2  = ComponentManager->GetComponent<cm::material>(testItem2);
	// *materialTestItem2  = { .diffuseTextureID_ = witchTexturehandle, .specularTextureID_ = container2SpecularTextureHandle, .ambient = { 0.05f, 0.05f, 0.05f },
	// 	.shininess = 128.0f * 0.078125f };
	
	// cm::material* materialInventory = ComponentManager->GetComponent<cm::material>(inventory);
	// *materialInventory = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2SpecularTextureHandle,
	// 	.ambient = { 0.05f, 0.05f, 0.05f }, .shininess = 128.0f * 0.078125f };
	
	// Entity font = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::transform, cm::font>(font);
	// *ComponentManager->GetComponent<cm::transform>(font) = { .tPosition = { 5.0f, 5.0f, 5.0f },
	// 	.fScale = 1.0f, .gltf = true };
	// ComponentManager->GetComponent<cm::mesh>(font)->handle = cubeHandle_OBJ;
	
	Entity directionalLight0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::actor, cm::mesh, cm::material, cm::directionalLight, cm::transform>(directionalLight0);
	*ComponentManager->GetComponent<cm::directionalLight>(directionalLight0) = { .position = { 0.0f, 10.0f, -15.0f },
		.direction = { 1.0f, -3.0f, 0.0f}, .ambient = { 0.05f, 0.05f, 0.05f }, .diffuse = {0.4f, 0.4f, 0.4f},
		.specular = {1.0f, 1.0f, 1.0f}};
 	*ComponentManager->GetComponent<cm::transform>(directionalLight0) = { .position = { 0.0f, 10.0f, -15.0f },
		.scale = 0.1f };
	ComponentManager->GetComponent<cm::mesh>(directionalLight0)->handle = hyperCubeHandle_GLTF;
	cm::material* materialDirectionalLight0  = ComponentManager->GetComponent<cm::material>(directionalLight0);
	*materialDirectionalLight0 = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2Texturehandle, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };

	// Entity directionalLight1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::directionalLight, cm::transform>(directionalLight1);
	// *ComponentManager->GetComponent<cm::directionalLight>(directionalLight1) = { .position = { 0.0f, 3.0f, 2.0f },
	// 	.direction = { 5.0f, -1.0f, 0.0f}, .ambient = { 0.05f, 0.05f, 0.05f }, .diffuse = {0.8f, 0.8f, 0.8f},
	// 	.specular = {1.0f, 1.0f, 1.0f}};
 	// *ComponentManager->GetComponent<cm::transform>(directionalLight1) = { .tPosition = { 0.0f, 3.0f, 2.0f },
	// 	.fScale = 0.2f };
	// ComponentManager->GetComponent<cm::mesh>(directionalLight1)->handle = hyperCubeHandle_GLTF;
	// cm::material* materialDirectionalLight1  = ComponentManager->GetComponent<cm::material>(directionalLight1);
	// *materialDirectionalLight1 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle, .ambient = { 0.05f, 0.05f, 0.0f },
	// 	.shininess = 128.0f * 0.078125f };

	// Entity directionalLight2 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::directionalLight, cm::transform>(directionalLight2);
	// *ComponentManager->GetComponent<cm::directionalLight>(directionalLight2) = { .position = { 3.0f, 3.0f, 0.0f },
	// 	.direction = { 1.0f, -1.0f, -5.0f}, .ambient = { 0.05f, 0.05f, 0.05f }, .diffuse = {0.8f, 0.8f, 0.8f},
	// 	.specular = {1.0f, 1.0f, 1.0f}};
 	// *ComponentManager->GetComponent<cm::transform>(directionalLight2) = { .tPosition = { 3.0f, 3.0f, 0.0f },
	// 	.fScale = 0.2f };
	// ComponentManager->GetComponent<cm::mesh>(directionalLight2)->handle = hyperCubeHandle_GLTF;
	// cm::material* materialDirectionalLight2  = ComponentManager->GetComponent<cm::material>(directionalLight2);
	// *materialDirectionalLight2 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle, .ambient = { 0.05f, 0.05f, 0.0f },
	// 	.shininess = 128.0f * 0.078125f };
	
	Entity pointLight0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::actor, cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight0);
	*ComponentManager->GetComponent<cm::pointLight>(pointLight0) = { .position = { 3.0f, 10.0f, 15.0f },
		.ambient = { 0.1f, 0.1f, 0.1f }, .diffuse = { 0.8f, 0.8f, 0.8f }, .specular = { 2.0f, 2.0f, 2.0f },
		.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
	*ComponentManager->GetComponent<cm::transform>(pointLight0) = { .position = { 3.0f, 10.0f, 15.0f }, .scale = 0.2f };
	ComponentManager->GetComponent<cm::mesh>(pointLight0)->handle = hyperCubeHandle_GLTF;
	cm::material* materialPointLight0   = ComponentManager->GetComponent<cm::material>(pointLight0);
	*materialPointLight0 = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2Texturehandle, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };

 	// Entity pointLight1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight1);
	// *ComponentManager->GetComponent<cm::pointLight>(pointLight1)  = { .position = { 0.0f, 3.0f, 0.0f },
	// 	.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.7f, 0.7f, 0.7f }, .specular = { 0.8f, 0.8f, 0.8f },
	// 	.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
	// *ComponentManager->GetComponent<cm::transform>(pointLight1) = { .tPosition = { 0.0f, 3.0f, 0.0f }, .fScale = 0.3f };
	// ComponentManager->GetComponent<cm::mesh>(pointLight1)->handle = hyperCubeHandle_GLTF;
	// cm::material* materialPointLight1 = ComponentManager->GetComponent<cm::material>(pointLight1);
	// *materialPointLight1 = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2Texturehandle };

	// Entity pointLight2 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight2);
	// *ComponentManager->GetComponent<cm::pointLight>(pointLight2)  = { .position = { 0.0f, 3.0f, 2.0f },
	// 	.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.7f, 0.7f, 0.7f }, .specular = { 0.8f, 0.8f, 0.8f },
	// 	.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
	// *ComponentManager->GetComponent<cm::transform>(pointLight2) = { .tPosition = { 0.0f, 3.0f, 2.0f }, .fScale = 0.3f };
	// ComponentManager->GetComponent<cm::mesh>(pointLight2)->handle = hyperCubeHandle_GLTF;
	// cm::material* materialPointLight2 = ComponentManager->GetComponent<cm::material>(pointLight2);
	// *materialPointLight2 = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2Texturehandle };

	// Entity pointLight3 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight3);
	// *ComponentManager->GetComponent<cm::pointLight>(pointLight3)  = { .position = { 2.0f, 3.0f, 0.0f },
	// 	.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.7f, 0.7f, 0.7f }, .specular = { 0.8f, 0.8f, 0.8f },
	// 	.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
	// *ComponentManager->GetComponent<cm::transform>(pointLight3) = { .tPosition = { 2.0f, 3.0f, 0.0f }, .fScale = 0.3f };
	// ComponentManager->GetComponent<cm::mesh>(pointLight3)->handle = hyperCubeHandle_GLTF;
	// cm::material* materialPointLight3 = ComponentManager->GetComponent<cm::material>(pointLight3);
	// *materialPointLight3 = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2Texturehandle };

	// Entity pointLight4 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight4);
	// *ComponentManager->GetComponent<cm::pointLight>(pointLight4)  = { .position = { 0.27f, 5.3f, 0.25f },
	// 	.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.7f, 0.7f, 0.7f }, .specular = { 0.8f, 0.8f, 0.8f },
	// 	.constant = 2.17f, .linear = 0.39f, .quadratic = 0.532f };
	// *ComponentManager->GetComponent<cm::transform>(pointLight4) = { .tPosition = { 0.5f, 3.0f, 0.8f }, .fScale = 0.3f };
	// ComponentManager->GetComponent<cm::mesh>(pointLight4)->handle = hyperCubeHandle_GLTF;
	// cm::material* materialPointLight4 = ComponentManager->GetComponent<cm::material>(pointLight4);
	// *materialPointLight4 = { .diffuseTextureID_ = container2Texturehandle, .specularTextureID_ = container2Texturehandle };
	
	Entity spotLight1 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::actor, cm::mesh, cm::material, cm::spotLight, cm::transform>(spotLight1);
	*ComponentManager->GetComponent<cm::spotLight>(spotLight1) = { .position = { -10.0f, 35.0f, -15.0f },
		.direction = { 0.0f, -3.0f, 2.0f }, .cutOff = 32.5f, .outerCutOff = 37.5f, .ambient = { 0.05f, 0.05f, 0.05f },
		.diffuse = { 3.8f, 3.8f, 3.8f }, .specular = { 5.0f, 5.0f, 5.0f }, .constant = 1.0f, .linear = 0.09f,
		.quadratic = 0.032f };
	*ComponentManager->GetComponent<cm::transform>(spotLight1) = { .position = { -10.0f, 35.0f, -15.0f }, .scale = 0.2f };
	ComponentManager->GetComponent<cm::mesh>(spotLight1)->handle = simpleCubeHandle_GLTF;
	cm::material* materialSpotLight1   = ComponentManager->GetComponent<cm::material>(spotLight1);
	*materialSpotLight1 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle };

	// Entity spotLight2 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::spotLight, cm::transform>(spotLight2);
	// *ComponentManager->GetComponent<cm::spotLight>(spotLight2) = { .position = { 0.0f, 3.0f, 10.0f },
	// 	.direction = { 0.0f, 0.0f, -5.0f }, .cutOff = 32.5f, .outerCutOff = 37.5f, .ambient = { 0.05f, 0.05f, 0.05f },
	// 	.diffuse = { 0.8f, 0.8f, 0.8f }, .specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f, .linear = 0.09f,
	// 	.quadratic = 0.032f };
	// *ComponentManager->GetComponent<cm::transform>(spotLight2) = { .tPosition = { 0.0f, 3.0f, 10.0f }, .fScale = 1.0f };
	// ComponentManager->GetComponent<cm::mesh>(spotLight2)->handle = simpleCubeHandle_GLTF;
	// cm::material* materialSpotLight2   = ComponentManager->GetComponent<cm::material>(spotLight2);
	// *materialSpotLight2 = { .diffuseTextureID_ = grayTextureHandle, .specularTextureID_ = grayTextureHandle };

	
    ///< Game rendering loop
//	Glvm->GameLoop(GLVM::core::OPENGL_RENDERER);
	GLVM->GameLoop();

	GLVM->GameKill();

	delete EntityManager;
	delete ComponentManager;
	delete GLVM;
	
    return 0;
}
