#include "ComponentManager.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Engine.hpp"
#include "MeshManager.hpp"
#include "SpritesData.hpp"
#include "TextureManager.hpp"
#include "Texture.hpp"
#include <math.h>

int main()
{
	using namespace GLVM;
	namespace cm  = GLVM::ecs::components;
	namespace ct = GAME_MECHANICS::ECS::components;

	ecs::EntityManager   * EntityManager     = ecs::EntityManager::GetInstance();
	ecs::ComponentManager* ComponentManager  = ecs::ComponentManager::GetInstance();
	core::MeshManager    * MeshManager       = core::MeshManager::GetInstance();
	ecs::TextureManager  * TextureManager    = ecs::TextureManager::GetInstance();
	ecs::TextureManager  * hudTextureManager = ecs::TextureManager::GetHUDInstance();
//    MeshManager->SetMesh("../waveFrontObj/cube_uv.obj");
	MeshManager->SetMesh("../waveFrontObj/cube2.obj");
	MeshManager->SetMesh("../waveFrontObj/sphere.obj");
	// MeshManager->SetMesh("../waveFrontObj/cone.obj");
	// MeshManager->SetMesh("../waveFrontObj/monkey.obj");
	MeshManager->SetMesh("../waveFrontObj/plain5.obj");
	// MeshManager->SetMesh("../waveFrontObj/chrismas_tree.obj");
	// MeshManager->SetMesh("../waveFrontObj/shama_final.obj");
	core::Engine* GLVM = core::Engine::GetInstance();		
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

    std::vector<ecs::Texture> TextureVector{ Texture_0, Texture_1, Texture_2, Texture_3, Texture_4 };

    TextureManager->SetTextureVector(TextureVector);
	
	ecs::Texture hudTexture_0{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = Crosshair_dat_len, .u_iData_ = Crosshair_dat };
    // CTexture hudTexture_1;
    // hudTexture_1.iWidth_ = 32;
    // hudTexture_1.iHeight_ = 32;
    // hudTexture_1.u_iData_ = witch_dat;
    // hudTexture_1.dat_length_ = witch_dat_len;
	
    std::vector<ecs::Texture> hudTextureVector;
    hudTextureVector.push_back(hudTexture_0);
    //    hudTextureVector.push_back(hudTexture_1);

    hudTextureManager->SetTextureVector(hudTextureVector);
//	core::Engine* GLVM = core::Engine::GetInstance();		
    Entity uiPlayer = EntityManager->CreateEntity();
    ComponentManager->CreateComponent<cm::vertex, ct::controller, cm::collider, cm::animation, cm::beholder,
									  cm::transform, cm::rigidBody, cm::event>(uiPlayer);
	*ComponentManager->GetComponent<cm::transform>(uiPlayer) = { .tPosition = { 5.7f, 2.5f, 2.0f }, .fScale = 1.0f };
	*ComponentManager->GetComponent<cm::rigidBody>(uiPlayer) = { .fMass_ = 6.0f };
    *ComponentManager->GetComponent<cm::beholder>(uiPlayer) = { .forward = { 0.0f, 0.0f, -1.0f },
		.up = { 0.0f, 1.0f, 0.0f } };
    ComponentManager->GetComponent<cm::vertex>(uiPlayer)->vkVertexId_ = 1;
	
	Entity plain0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::vertex, cm::transform>(plain0);
	*ComponentManager->GetComponent<cm::transform>(plain0) = { .tPosition = { 5.5f, 6.5f, 0.0f }, .fScale = 1.2f };
    ComponentManager->GetComponent<cm::vertex>(plain0)->vkVertexId_ = 2;
	cm::material* materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
	*materialPlain0 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
    TextureManager->BindTexture(plain0, materialPlain0->diffuseTextureID_);
    
	Entity uiWitch = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::vertex, cm::collider, cm::transform>(uiWitch);
	*ComponentManager->GetComponent<cm::transform>(uiWitch) = { .tPosition = { 0.5f, 1.0f, 0.5f }, .fScale = 1.0f };
    ComponentManager->GetComponent<cm::vertex>(uiWitch)->vkVertexId_ = 0;
	cm::material* materialWitch  = ComponentManager->GetComponent<cm::material>(uiWitch);
	*materialWitch  = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
    TextureManager->BindTexture(uiWitch, materialWitch->diffuseTextureID_);

    // Entity uiWitch2 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::material, cm::vertex, cm::collider, cm::transform>(uiWitch2);
	// ComponentManager->GetComponent<cm::vertex>(uiWitch2)->vkVertexId_ = 0;
	// *ComponentManager->GetComponent<cm::transform>(uiWitch2) = { .tPosition
	// 	= { 5.5f, 5.5f, 1.5f }, .fScale = 1.0f };
	// cm::material* materialWitch2  = ComponentManager->GetComponent<cm::material>(uiWitch2);
	// *materialWitch2 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
	// 	.shininess = 128.0f * 0.078125f };
    // TextureManager->BindTexture(uiWitch2, materialWitch2->diffuseTextureID_);

	// Entity uiWitch3 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::material, cm::vertex, cm::collider, cm::transform>(uiWitch3);
	// ComponentManager->GetComponent<cm::vertex>(uiWitch3)->vkVertexId_ = 0;
	// *ComponentManager->GetComponent<cm::transform>(uiWitch3) = { .tPosition = { 3.7f, 2.5f, 3.0f }, .fScale = 2.0f };
	// cm::material* materialWitch3  = ComponentManager->GetComponent<cm::material>(uiWitch3);
	// *materialWitch3 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
	// 	.shininess = 128.0f * 0.078125f };
    // TextureManager->BindTexture(uiWitch3, materialWitch3->diffuseTextureID_);

	Entity uiWitch4 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::vertex, cm::collider, cm::transform>(uiWitch4);
	ComponentManager->GetComponent<cm::vertex>(uiWitch4)->vkVertexId_ = 0;
//	*ComponentManager->GetComponent<cm::rigidBody>(uiWitch4) = { .fMass_ = 4.0f };
	*ComponentManager->GetComponent<cm::transform>(uiWitch4) = { .tPosition = { 5.7f, 1.0f, 2.0f }, .fScale = 2.0f };
	cm::material* materialWitch4  = ComponentManager->GetComponent<cm::material>(uiWitch4);
	*materialWitch4 = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
    TextureManager->BindTexture(uiWitch4, materialWitch4->diffuseTextureID_);
	
    // Entity u_iHud1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::vertex, cm::material, cm::collider, cm::transform>(u_iHud1);
    // ComponentManager->GetComponent<cm::vertex>(u_iHud1)->vkVertexId_ = 0;
	// *ComponentManager->GetComponent<cm::transform>(u_iHud1) = { .tPosition = { 0.0, 0.0f, 0.0f }, .fScale = 0.1f , .hud = true };
	// cm::material* materialHud0   = ComponentManager->GetComponent<cm::material>(u_iHud1);
	// *materialHud0 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
    // hudTextureManager->BindTexture(u_iHud1, materialHud0->diffuseTextureID_);

	// Entity directionalLight0 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::vertex, cm::material, cm::directionalLight, cm::transform>(directionalLight0);
	// *ComponentManager->GetComponent<cm::directionalLight>(directionalLight0) = { .position = { 2.0f, 5.0f, -8.0f },
	// 	.direction = { 0.0f, 1.5f, -1.0f}, .ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = {0.5f, 0.5f, 0.5f},
	// 	.specular = {0.3f, 0.3f, 0.3f}};
	// *ComponentManager->GetComponent<cm::transform>(directionalLight0) = { .tPosition = { 2.0f, 5.0f, -8.0f },
	// 	.fScale = 0.3f };
	// ComponentManager->GetComponent<cm::vertex>(directionalLight0)->vkVertexId_ = 0;
	// cm::material* materialDirectionalLight0  = ComponentManager->GetComponent<cm::material>(directionalLight0);
	// *materialDirectionalLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	// TextureManager->BindTexture(directionalLight0, materialDirectionalLight0->diffuseTextureID_);

	// Entity directionalLight1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::vertex, cm::material, cm::directionalLight, cm::transform>(directionalLight1);
	// ComponentManager->GetComponent<cm::directionalLight>(directionalLight1) = { .position = { 20.0f, 5.0f, 5.0f },
	// 	.direction = { 0.0f, 1.5f, -1.0f}, .ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = {0.5f, 0.5f, 0.5f},
	// 	.specular = {0.0f, 0.0f, 0.0f}};
	// ComponentManager->GetComponent<cm::transform>(directionalLight1) = { .tPosition = { 20.0f, 5.0f, 5.0f },
	// 	.fScale = 0.3f };
	// ComponentManager->GetComponent<cm::vertex>(directionalLight1).vkVertexId_ = 0;
	// cm::material                  & materialDirectionalLight1  = ComponentManager->GetComponent<cm::material>(directionalLight1);
	// materialDirectionalLight1 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	// TextureManager->BindTexture(directionalLight1, materialDirectionalLight1.diffuseTextureID_);
	
	// Entity pointLight0 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::vertex, cm::material, cm::pointLight, cm::transform>(pointLight0);
	// ComponentManager->GetComponent<cm::pointLight>(pointLight0) = { .position = { 0.0f, 3.3f, 2.7f },
	// 	.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.0f, 1.0f, 1.0f },
	// 	.constant = 1.0f, .linear = 0.09f, .quadratic = 0.1f };
	// ComponentManager->GetComponent<cm::transform>(pointLight0) = { .tPosition = { 0.0f, 3.3f, 2.7f }, .fScale = 0.3f };
	// ComponentManager->GetComponent<cm::vertex>(pointLight0).vkVertexId_ = 0;
	// cm::material& materialPointLight0   = ComponentManager->GetComponent<cm::material>(pointLight0);
	// materialPointLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	// TextureManager->BindTexture(pointLight0, materialPointLight0.diffuseTextureID_);

	Entity pointLight1 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::pointLight, cm::transform>(pointLight1);
	*ComponentManager->GetComponent<cm::pointLight>(pointLight1)  = { .position = { 5.2f, 5.3f, 5.5f },
		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 0.3f, 0.3f, 0.3f },
		.constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f };
	*ComponentManager->GetComponent<cm::transform>(pointLight1) = { .tPosition = { 5.2f, 5.3f, 5.5f }, .fScale = 0.3f };
	ComponentManager->GetComponent<cm::vertex>(pointLight1)->vkVertexId_ = 0;
	cm::material* materialPointLight1 = ComponentManager->GetComponent<cm::material>(pointLight1);
	*materialPointLight1 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	TextureManager->BindTexture(pointLight1, materialPointLight1->diffuseTextureID_);

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
	
// 	Entity spotLight0 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::spotLight, cm::transform>(spotLight0);
// 	*ComponentManager->GetComponent<cm::spotLight>(spotLight0) = { .position = { -3.0f, 3.0f, 1.0f },
// 		.direction = { 3.0f, -1.5f, -0.5f }, .cutOff = 12.5f, .outerCutOff = 17.5f, .ambient = { 0.2f, 0.2f, 0.2f },
// 		.diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 0.3f, 0.3f, 0.3f }, .constant = 1.0f, .linear = 0.09f,
// 		.quadratic = 0.032f };
// 	*ComponentManager->GetComponent<cm::transform>(spotLight0) = { .tPosition = { -3.0f, 3.0f, 1.0f }, .fScale = 0.3f };
// 	ComponentManager->GetComponent<cm::vertex>(spotLight0)->vkVertexId_ = 0;
// 	cm::material* materialSpotLight0   = ComponentManager->GetComponent<cm::material>(spotLight0);
// 	*materialSpotLight0 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
// 	TextureManager->BindTexture(spotLight0, materialSpotLight0->diffuseTextureID_);
// //	spotLightComponent0.position     =  TransformPlayer.tPosition;
// //	spotLightComponent0.direction    =  TransformPlayer.tForward;
// //	transformSpotLight0 = { .tPosition = transformPlayer.tPosition, .fScale = 0.3f };

// 	Entity spotLight1 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::spotLight, cm::transform>(spotLight1);
// 	*ComponentManager->GetComponent<cm::spotLight>(spotLight1) = { .position = { 1.0f, 3.0f, 4.0f },
// 		.direction = { -0.5f, -1.5f, -3.5f }, .cutOff = 12.5f, .outerCutOff = 17.5f, .ambient = { 0.2f, 0.2f, 0.2f },
// 		.diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 0.3f, 0.3f, 0.3f }, .constant = 1.0f, .linear = 0.09f,
// 		.quadratic = 0.032f };
// 	*ComponentManager->GetComponent<cm::transform>(spotLight1) = { .tPosition = { 1.0f, 3.0f, 4.0f }, .fScale = 0.3f };
// 	ComponentManager->GetComponent<cm::vertex>(spotLight1)->vkVertexId_ = 0;
// 	cm::material* materialSpotLight1   = ComponentManager->GetComponent<cm::material>(spotLight1);
// 	*materialSpotLight1 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
// 	TextureManager->BindTexture(spotLight1, materialSpotLight1->diffuseTextureID_);
//	spotLightComponent0.position     =  TransformPlayer.tPosition;
//	spotLightComponent0.direction    =  TransformPlayer.tForward;
//	transformSpotLight0 = { .tPosition = transformPlayer.tPosition, .fScale = 0.3f };


	
    ///< Game rendering loop
	GLVM->GameLoop();
	GLVM->GameKill();

    return 0;
}
