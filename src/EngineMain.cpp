#include "Components/ControllerComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Engine.hpp"
#include "SpritesData.hpp"

int main()
{
	using namespace GLVM;
	namespace cm  = GLVM::ecs::components;
	namespace ct = GAME_MECHANICS::ECS::components;

	ecs::EntityManager   * EntityManager     = ecs::EntityManager::GetInstance();
	ecs::ComponentManager* ComponentManager  = ecs::ComponentManager::GetInstance();
//	core::MeshManager    * MeshManager       = core::MeshManager::GetInstance();
//	ecs::TextureManager  * hudTextureManager = ecs::TextureManager::GetHUDInstance();
//    MeshManager->SetMesh("../waveFrontObj/cube_uv.obj");
//	MeshManager->SetMesh("../waveFrontObj/cube2.obj");
//	MeshManager->SetMesh("../waveFrontObj/sphere.obj");
	// MeshManager->SetMesh("../waveFrontObj/cone.obj");
	// MeshManager->SetMesh("../waveFrontObj/monkey.obj");
//	MeshManager->SetMesh("../waveFrontObj/plain5.obj");
//	MeshManager->SetMesh("../waveFrontObj/ray.obj");
//	MeshManager->SetMesh("../waveFrontObj/ray_y.obj");
	// MeshManager->SetMesh("../waveFrontObj/chrismas_tree.obj");
	// MeshManager->SetMesh("../waveFrontObj/shama_final.obj");

//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/cube2.gltf");
	// MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/sphere.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake2.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake4.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake5.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake6.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake7.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake8.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/cubeAnimation.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake9.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake_x_inverse_y.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake32.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/snake9.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/bone_cube.gltf");
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/cb.gltf");	
//	MeshManager->SetMeshGLTF("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/stick.gltf");
	
//	core::Engine* GLVM = core::Engine::GetInstance();		
	ecs::Texture Texture_0{ .iWidth_ = 128, .iHeight_ = 96,
		.dat_length_ = chelik_dat_len, .u_iData_ = chelik_dat };
	ecs::Texture Texture_1{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = witch_dat_len, .u_iData_ = witch_dat };
	ecs::Texture Texture_2{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = gray_dat_len, .u_iData_ = gray_dat };
	ecs::Texture Texture_3{ .iWidth_ = 500, .iHeight_ = 500,
		.dat_length_ = container2_dat_len, .u_iData_ = container2_dat };
	ecs::Texture Texture_4{ .iWidth_ = 500, .iHeight_ = 500,
		.dat_length_ = container2_specular_dat_len, .u_iData_ = container2_specular_dat };
	ecs::Texture hudTexture_0{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = Crosshair_dat_len, .u_iData_ = Crosshair_dat };

    std::vector<ecs::Texture> TextureVector{ Texture_0, Texture_1, Texture_2, Texture_3, Texture_4, hudTexture_0 };
//    TextureManager->SetTextureVector(TextureVector);

    // CTexture hudTexture_1;
    // hudTexture_1.iWidth_ = 32;
    // hudTexture_1.iHeight_ = 32;
    // hudTexture_1.u_iData_ = witch_dat;
    // hudTexture_1.dat_length_ = witch_dat_len;
	
    // std::vector<ecs::Texture> hudTextureVector;
    // hudTextureVector.push_back(hudTexture_0);
    //    hudTextureVector.push_back(hudTexture_1);

//    hudTextureManager->SetTextureVector(hudTextureVector);
//	core::Engine* GLVM = core::Engine::GetInstance();		
    Entity uiPlayer = EntityManager->CreateEntity();
    ComponentManager->CreateComponent<cm::mesh, ct::controller, cm::collider, cm::animation, cm::beholder,
		cm::transform, cm::rigidBody, cm::event>(uiPlayer);
	// *ComponentManager->GetComponent<cm::directionalLight>(uiPlayer) = { .position = { 15.7f, 7.5f, 2.0f },
	// 	.direction = { 0.0f, 1.0f, 3.0f}, .ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = {0.5f, 0.5f, 0.5f},
	// 	.specular = {0.3f, 0.3f, 0.3f}};
	*ComponentManager->GetComponent<cm::transform>(uiPlayer) = { .tPosition = { 5.7f, 10.0f, 7.0f }, .fScale = 0.1f };
	*ComponentManager->GetComponent<cm::rigidBody>(uiPlayer) = { .fMass_ = 6.0f };
    *ComponentManager->GetComponent<cm::beholder>(uiPlayer) = { .forward = { 0.0f, 0.0f, -1.0f },
		.up = { 0.0f, 1.0f, 0.0f } };
    ComponentManager->GetComponent<cm::mesh>(uiPlayer)->id = 0;
	// cm::material* materialPlayer  = ComponentManager->GetComponent<cm::material>(uiPlayer);
	// *materialPlayer = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
	// 	.shininess = 512.0f * 1.078125f };
	
	Entity plain0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::transform, cm::collider>(plain0);
	*ComponentManager->GetComponent<cm::transform>(plain0) = { .tPosition = { 0.0f, 0.5f, 0.0f }, .yaw = 10.0f, .pitch = 0.0f, .fScale = 5.2f };
    ComponentManager->GetComponent<cm::mesh>(plain0)->id = 0;
	cm::material* materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
	*materialPlain0 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
//    TextureManager->BindTexture(plain0, materialPlain0->diffuseTextureID_);

	for ( u32 i = 0; i < 5; ++i ) {
	Entity uiWitch = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(uiWitch);
//	ComponentManager->GetComponent<cm::texture>(uiWitch)->id = 1;
	*ComponentManager->GetComponent<cm::transform>(uiWitch) = { .tPosition = { (float)i, 10.0f, 0.0f },
		.yaw = 0.0f, .pitch = 0.0f, .fScale = 1.2f };
	ComponentManager->GetComponent<cm::mesh>(uiWitch)->id = 1;
	cm::material* materialWitch  = ComponentManager->GetComponent<cm::material>(uiWitch);
	*materialWitch  = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.05f },
		.shininess = 128.0f * 0.078125f };
	}

 	Entity cube0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(cube0);
//	ComponentManager->GetComponent<cm::texture>(uiWitch)->id = 1;
	*ComponentManager->GetComponent<cm::transform>(cube0) = { .tPosition = { 7.0f, 3.0f, 0.0f },
		.yaw = 0.0f, .pitch = 0.0f, .fScale = 1.0f };
    ComponentManager->GetComponent<cm::mesh>(cube0)->id = 0;
	cm::material* materialCube0  = ComponentManager->GetComponent<cm::material>(cube0);
	*materialCube0  = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.05f },
		.shininess = 128.0f * 0.078125f };
	
//     Entity uiWitch2 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(uiWitch2);
// //	ComponentManager->GetComponent<cm::texture>(uiWitch2)->id = 0;
// 	ComponentManager->GetComponent<cm::mesh>(uiWitch2)->id = 0;
// 	*ComponentManager->GetComponent<cm::transform>(uiWitch2) = { .tPosition
// 		= { 0.0f, -5.0f, 0.0f }, .fScale = 0.6f };
// 	cm::material* materialWitch2  = ComponentManager->GetComponent<cm::material>(uiWitch2);
// 	*materialWitch2 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
// 		.shininess = 128.0f * 0.078125f };

//  	Entity cube1 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(cube1);
// //	ComponentManager->GetComponent<cm::texture>(uiWitch)->id = 1;
// 	*ComponentManager->GetComponent<cm::transform>(cube1) = { .tPosition = { 0.0f, -18.0f, 0.0f },
// 		.yaw = 0.0f, .pitch = 0.0f, .fScale = 1.0f };
//     ComponentManager->GetComponent<cm::mesh>(cube1)->id = 0;
// 	cm::material* materialCube1  = ComponentManager->GetComponent<cm::material>(cube1);
// 	*materialCube1  = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.05f },
// 		.shininess = 128.0f * 0.078125f };
	
// 	Entity uiWitch3 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(uiWitch3);
// 	ComponentManager->GetComponent<cm::mesh>(uiWitch3)->id = 0;
// 	*ComponentManager->GetComponent<cm::transform>(uiWitch3) = { .tPosition = { 5.0f, 0.0f, 0.0f }, .fScale = 0.6f };
// 	cm::material* materialWitch3  = ComponentManager->GetComponent<cm::material>(uiWitch3);
// 	*materialWitch3 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
// 		.shininess = 128.0f * 0.078125f };

//  	Entity cube2 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(cube2);
// //	ComponentManager->GetComponent<cm::texture>(uiWitch)->id = 1;
// 	*ComponentManager->GetComponent<cm::transform>(cube2) = { .tPosition = { 18.0f, 0.0f, 0.0f },
// 		.yaw = 0.0f, .pitch = 0.0f, .fScale = 1.0f };
//     ComponentManager->GetComponent<cm::mesh>(cube2)->id = 0;
// 	cm::material* materialCube2  = ComponentManager->GetComponent<cm::material>(cube2);
// 	*materialCube2  = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.05f },
// 		.shininess = 128.0f * 0.078125f };
	
// 	Entity uiWitch4 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(uiWitch4);
// 	ComponentManager->GetComponent<cm::mesh>(uiWitch4)->id = 0;
// //	*ComponentManager->GetComponent<cm::rigidBody>(uiWitch4) = { .fMass_ = 4.0f };
// 	*ComponentManager->GetComponent<cm::transform>(uiWitch4) = { .tPosition = { -5.0f, 0.0f, 0.0f }, .fScale = 0.2f };
// 	cm::material* materialWitch4  = ComponentManager->GetComponent<cm::material>(uiWitch4);
// 	*materialWitch4 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0, .ambient = { 0.05f, 0.05f, 0.0f },
// 		.shininess = 128.0f * 0.078125f };

// 	Entity cube3 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(cube3);
// 	ComponentManager->GetComponent<cm::mesh>(cube3)->id = 0;
// //	*ComponentManager->GetComponent<cm::rigidBody>(uiWitch4) = { .fMass_ = 4.0f };
// 	*ComponentManager->GetComponent<cm::transform>(cube3) = { .tPosition = { -18.0f, 0.0f, 0.0f }, .fScale = 0.1f };
// 	cm::material* materialCube3  = ComponentManager->GetComponent<cm::material>(cube3);
// 	*materialCube3 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
// 		.shininess = 128.0f * 0.078125f };

	
    // Entity u_iHud1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::vertex, cm::material, cm::collider, cm::transform, cm::texture>(u_iHud1);
	// ComponentManager->GetComponent<cm::texture>(u_iHud1)->id = 3;
    // ComponentManager->GetComponent<cm::vertex>(u_iHud1)->vkVertexId_ = 0;
	// *ComponentManager->GetComponent<cm::transform>(u_iHud1) = { .tPosition = { 0.0, 0.0f, 0.0f }, .fScale = 0.1f , .hud = true };
	// cm::material* materialHud0   = ComponentManager->GetComponent<cm::material>(u_iHud1);
	// *materialHud0 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
    // TextureManager->BindTexture(u_iHud1, materialHud0->diffuseTextureID_);

	Entity directionalLight0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::mesh, cm::material, cm::directionalLight, cm::transform>(directionalLight0);
	*ComponentManager->GetComponent<cm::directionalLight>(directionalLight0) = { .position = { 10.0f, 15.0f, 0.0f },
		.direction = { -5.0f, -3.0f, 0.0f}, .ambient = { 0.05f, 0.05f, 0.05f }, .diffuse = {0.8f, 0.8f, 0.8f},
		.specular = {1.0f, 1.0f, 1.0f}};
 	*ComponentManager->GetComponent<cm::transform>(directionalLight0) = { .tPosition = { 10.0f, 15.0f, 0.0f },
		.fScale = 0.2f };
	ComponentManager->GetComponent<cm::mesh>(directionalLight0)->id = 0;
	cm::material* materialDirectionalLight0  = ComponentManager->GetComponent<cm::material>(directionalLight0);
	*materialDirectionalLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };

	// Entity directionalLight1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::directionalLight, cm::transform>(directionalLight1);
	// *ComponentManager->GetComponent<cm::directionalLight>(directionalLight1) = { .position = { 0.0f, 3.0f, 2.0f },
	// 	.direction = { 5.0f, -1.0f, 0.0f}, .ambient = { 0.05f, 0.05f, 0.05f }, .diffuse = {0.8f, 0.8f, 0.8f},
	// 	.specular = {1.0f, 1.0f, 1.0f}};
 	// *ComponentManager->GetComponent<cm::transform>(directionalLight1) = { .tPosition = { 0.0f, 3.0f, 2.0f },
	// 	.fScale = 0.2f };
	// ComponentManager->GetComponent<cm::mesh>(directionalLight1)->id = 0;
	// cm::material* materialDirectionalLight1  = ComponentManager->GetComponent<cm::material>(directionalLight1);
	// *materialDirectionalLight1 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
	// 	.shininess = 128.0f * 0.078125f };

	// Entity directionalLight2 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::directionalLight, cm::transform>(directionalLight2);
	// *ComponentManager->GetComponent<cm::directionalLight>(directionalLight2) = { .position = { 3.0f, 3.0f, 0.0f },
	// 	.direction = { 1.0f, -1.0f, -5.0f}, .ambient = { 0.05f, 0.05f, 0.05f }, .diffuse = {0.8f, 0.8f, 0.8f},
	// 	.specular = {1.0f, 1.0f, 1.0f}};
 	// *ComponentManager->GetComponent<cm::transform>(directionalLight2) = { .tPosition = { 3.0f, 3.0f, 0.0f },
	// 	.fScale = 0.2f };
	// ComponentManager->GetComponent<cm::mesh>(directionalLight2)->id = 1;
	// cm::material* materialDirectionalLight2  = ComponentManager->GetComponent<cm::material>(directionalLight2);
	// *materialDirectionalLight2 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
	// 	.shininess = 128.0f * 0.078125f };
	
// 	Entity cube4 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(cube4);
// 	ComponentManager->GetComponent<cm::mesh>(cube4)->id = 0;
// //	*ComponentManager->GetComponent<cm::rigidBody>(uiWitch4) = { .fMass_ = 4.0f };
// 	*ComponentManager->GetComponent<cm::transform>(cube4) = { .tPosition = { 0.0f, 0.0f, 18.0f }, .fScale = 1.0f };
// 	cm::material* materialCube4  = ComponentManager->GetComponent<cm::material>(cube4);
// 	*materialCube4 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
// 		.shininess = 128.0f * 0.078125f };
	
	// Entity directionalLight1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::directionalLight, cm::transform>(directionalLight1);
	// *ComponentManager->GetComponent<cm::directionalLight>(directionalLight1) = { .position = { 0.0f, 2.0f, 2.0f },
	// 	.direction = { 0.0f, 1.5f, -1.0f}, .ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = {0.5f, 0.5f, 0.5f},
	// 	.specular = {0.0f, 0.0f, 0.0f}};
	// *ComponentManager->GetComponent<cm::transform>(directionalLight1) = { .tPosition = { 0.0f, 2.0f, 2.0f },
	// 	.fScale = 0.3f };
	// ComponentManager->GetComponent<cm::mesh>(directionalLight1)->id = 0;
	// cm::material* materialDirectionalLight1  = ComponentManager->GetComponent<cm::material>(directionalLight1);
	// *materialDirectionalLight1 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
//	TextureManager->BindTexture(directionalLight1, materialDirectionalLight1.diffuseTextureID_);
	
	Entity pointLight0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight0);
	*ComponentManager->GetComponent<cm::pointLight>(pointLight0) = { .position = { 5.0f, 17.0f, 2.0f },
		.ambient = { 0.1f, 0.1f, 0.1f }, .diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.0f, 1.0f, 1.0f },
		.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
	*ComponentManager->GetComponent<cm::transform>(pointLight0) = { .tPosition = { 5.0f, 17.0f, 2.0f }, .fScale = 0.2f };
	ComponentManager->GetComponent<cm::mesh>(pointLight0)->id = 0;
	cm::material* materialPointLight0   = ComponentManager->GetComponent<cm::material>(pointLight0);
	*materialPointLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };

//  	Entity pointLight1 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight1);
// //	ComponentManager->GetComponent<cm::texture>(pointLight1)->id = 0;
// 	*ComponentManager->GetComponent<cm::pointLight>(pointLight1)  = { .position = { 0.0f, 3.0f, 0.0f },
// 		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.7f, 0.7f, 0.7f }, .specular = { 0.8f, 0.8f, 0.8f },
// 		.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
// 	*ComponentManager->GetComponent<cm::transform>(pointLight1) = { .tPosition = { 0.0f, 3.0f, 0.0f }, .fScale = 0.3f };
// 	ComponentManager->GetComponent<cm::mesh>(pointLight1)->id = 0;
// 	cm::material* materialPointLight1 = ComponentManager->GetComponent<cm::material>(pointLight1);
// 	*materialPointLight1 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };

// 	Entity pointLight2 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight2);
// //	ComponentManager->GetComponent<cm::texture>(pointLight1)->id = 0;
// 	*ComponentManager->GetComponent<cm::pointLight>(pointLight2)  = { .position = { 0.0f, 3.0f, 2.0f },
// 		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.7f, 0.7f, 0.7f }, .specular = { 0.8f, 0.8f, 0.8f },
// 		.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
// 	*ComponentManager->GetComponent<cm::transform>(pointLight2) = { .tPosition = { 0.0f, 3.0f, 2.0f }, .fScale = 0.3f };
// 	ComponentManager->GetComponent<cm::mesh>(pointLight2)->id = 0;
// 	cm::material* materialPointLight2 = ComponentManager->GetComponent<cm::material>(pointLight2);
// 	*materialPointLight2 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };

// 	Entity pointLight3 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight3);
// //	ComponentManager->GetComponent<cm::texture>(pointLight1)->id = 0;
// 	*ComponentManager->GetComponent<cm::pointLight>(pointLight3)  = { .position = { 2.0f, 3.0f, 0.0f },
// 		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.7f, 0.7f, 0.7f }, .specular = { 0.8f, 0.8f, 0.8f },
// 		.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
// 	*ComponentManager->GetComponent<cm::transform>(pointLight3) = { .tPosition = { 2.0f, 3.0f, 0.0f }, .fScale = 0.3f };
// 	ComponentManager->GetComponent<cm::mesh>(pointLight3)->id = 0;
// 	cm::material* materialPointLight3 = ComponentManager->GetComponent<cm::material>(pointLight3);
// 	*materialPointLight3 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };

// 	Entity pointLight4 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::mesh, cm::material, cm::pointLight, cm::transform>(pointLight4);
// //	ComponentManager->GetComponent<cm::texture>(pointLight1)->id = 0;
// 	*ComponentManager->GetComponent<cm::pointLight>(pointLight4)  = { .position = { 0.27f, 5.3f, 0.25f },
// 		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.7f, 0.7f, 0.7f }, .specular = { 0.8f, 0.8f, 0.8f },
// 		.constant = 2.17f, .linear = 0.39f, .quadratic = 0.532f };
// 	*ComponentManager->GetComponent<cm::transform>(pointLight4) = { .tPosition = { 0.5f, 3.0f, 0.8f }, .fScale = 0.3f };
// 	ComponentManager->GetComponent<cm::mesh>(pointLight4)->id = 0;
// 	cm::material* materialPointLight4 = ComponentManager->GetComponent<cm::material>(pointLight4);
// 	*materialPointLight4 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	
// 	Entity pointLight2 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::pointLight, cm::transform>(pointLight2);
// 	*ComponentManager->GetComponent<cm::pointLight>(pointLight2) = { .position = { 9.2f, 3.3f, 7.5f },
// 		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 0.3f, 0.3f, 0.3f },
// 		.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
// 	*ComponentManager->GetComponent<cm::transform>(pointLight2) = { .tPosition = { 9.2f, 3.3f, 7.5f }, .fScale = 0.3f };
// 	ComponentManager->GetComponent<cm::vertex>(pointLight2)->vkVertexId_ = 0;
// 	cm::material* materialPointLight2 = ComponentManager->GetComponent<cm::material>(pointLight2);
// 	*materialPointLight2 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
// 	TextureManager->BindTexture(pointLight2, materialPointLight2->diffuseTextureID_);
	
	// Entity spotLight0 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::spotLight, cm::transform>(spotLight0);
	// *ComponentManager->GetComponent<cm::spotLight>(spotLight0) = { .position = { -3.0f, 6.0f, 1.0f },
	// 	.direction = { 3.0f, 0.0f, 1.0f }, .cutOff = 25.0f, .outerCutOff = 27.0f, .ambient = { 0.2f, 0.2f, 0.2f },
	// 	.diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 0.3f, 0.3f, 0.3f }, .constant = 1.0f, .linear = 0.09f,
	// 	.quadratic = 0.032f };
	// *ComponentManager->GetComponent<cm::transform>(spotLight0) = { .tPosition = { -3.0f, 6.0f, 1.0f }, .fScale = 0.3f };
	// ComponentManager->GetComponent<cm::mesh>(spotLight0)->id = 0;
	// cm::material* materialSpotLight0   = ComponentManager->GetComponent<cm::material>(spotLight0);
	// *materialSpotLight0 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
	// spotLightComponent0.position     =  TransformPlayer.tPosition;
	// spotLightComponent0.direction    =  TransformPlayer.tForward;
	// transformSpotLight0 = { .tPosition = transformPlayer.tPosition, .fScale = 0.3f };

	Entity spotLight1 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::mesh, cm::material, cm::spotLight, cm::transform>(spotLight1);
	*ComponentManager->GetComponent<cm::spotLight>(spotLight1) = { .position = { 1.0f, 15.0f, -5.0f },
		.direction = { 0.0f, -1.0f, 1.0f }, .cutOff = 32.5f, .outerCutOff = 37.5f, .ambient = { 0.05f, 0.05f, 0.05f },
		.diffuse = { 0.8f, 0.8f, 0.8f }, .specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f, .linear = 0.09f,
		.quadratic = 0.032f };
	*ComponentManager->GetComponent<cm::transform>(spotLight1) = { .tPosition = { 1.0f, 15.0f, -5.0f }, .fScale = 0.5f };
	ComponentManager->GetComponent<cm::mesh>(spotLight1)->id = 0;
	cm::material* materialSpotLight1   = ComponentManager->GetComponent<cm::material>(spotLight1);
	*materialSpotLight1 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2 };

	// Entity spotLight2 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::mesh, cm::material, cm::spotLight, cm::transform>(spotLight2);
	// *ComponentManager->GetComponent<cm::spotLight>(spotLight2) = { .position = { 0.0f, 3.0f, 10.0f },
	// 	.direction = { 0.0f, 0.0f, -5.0f }, .cutOff = 32.5f, .outerCutOff = 37.5f, .ambient = { 0.05f, 0.05f, 0.05f },
	// 	.diffuse = { 0.8f, 0.8f, 0.8f }, .specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f, .linear = 0.09f,
	// 	.quadratic = 0.032f };
	// *ComponentManager->GetComponent<cm::transform>(spotLight2) = { .tPosition = { 0.0f, 3.0f, 10.0f }, .fScale = 1.0f };
	// ComponentManager->GetComponent<cm::mesh>(spotLight2)->id = 0;
	// cm::material* materialSpotLight2   = ComponentManager->GetComponent<cm::material>(spotLight2);
	// *materialSpotLight2 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2 };
	
// 	Entity cube5 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::material, cm::mesh, cm::collider, cm::transform>(cube5);
// 	ComponentManager->GetComponent<cm::mesh>(cube5)->id = 0;
// //	*ComponentManager->GetComponent<cm::rigidBody>(uiWitch4) = { .fMass_ = 4.0f };
// 	*ComponentManager->GetComponent<cm::transform>(cube5) = { .tPosition = { 0.0f, 0.0f, -18.0f }, .fScale = 30.0f };
// 	cm::material* materialCube5  = ComponentManager->GetComponent<cm::material>(cube5);
// 	*materialCube5 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
// 		.shininess = 128.0f * 0.078125f };
	
	// TextureManager->BindTexture(spotLight1, materialSpotLight1->diffuseTextureID_);
	// spotLightComponent0.position     =  TransformPlayer.tPosition;
	// spotLightComponent0.direction    =  TransformPlayer.tForward;
	// transformSpotLight0 = { .tPosition = transformPlayer.tPosition, .fScale = 0.3f };

	core::Engine* GLVM = core::Engine::GetInstance();
	GLVM->SetTextureVector(TextureVector);
	GLVM->SetMesh("../waveFrontObj/cube.obj");
	GLVM->SetMesh("../waveFrontObj/cone.obj");
//	GLVM->SetMesh("../waveFrontObj/plain5.obj");
	GLVM->SetMesh("../waveFrontObj/ico_sphere.obj");
//	GLVM->SetMesh("../waveFrontObj/shama_final.obj");
	GLVM->SetMesh("../waveFrontObj/monkey.obj");
	GLVM->SetMesh("../waveFrontObj/uv_sphere.obj");
	GLVM->SetMesh("../waveFrontObj/torus.obj");
	GLVM->SetMesh("../waveFrontObj/pipe.obj");
//	GLVM->SetMeshGLTF("../gltf/snake32.gltf");
//	GLVM->SetMeshGLTF("../gltf/cone_new.gltf");
//	GLVM->SetMeshGLTF("../gltf/chel5.gltf");
	GLVM->SetMeshGLTF("../gltf/hyper_cube.gltf");
	GLVM->SetMeshGLTF("../gltf/mega_chel.gltf");
//	GLVM->SetMeshGLTF("../gltf/Fox.gltf");
//	GLVM->SetMeshGLTF("../gltf/chel3.gltf");
//	GLVM->SetMeshGLTF("../gltf/ray.gltf");
	
    ///< Game rendering loop
//	GLVM->GameLoop(GLVM::core::OPENGL_RENDERER);
	GLVM->GameLoop(GLVM::core::VULKAN_RENDERER);

	GLVM->GameKill();

    return 0;
}
