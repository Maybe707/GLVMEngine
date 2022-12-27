#include "ComponentManager.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Engine.hpp"
#include "MeshManager.hpp"
#include "SpritesData.hpp"
#include "TextureManager.hpp"
#include "Texture.hpp"
#include <math.h>

using namespace GLVM::Core;
using namespace GLVM::ECS;
using namespace GLVM::ECS::Components;

int main()
{
    CEntityManager   * EntityManager     = CEntityManager::GetInstance();
	CComponentManager* ComponentManager  = CComponentManager::GetInstance();
    CMeshManager     * MeshManager       = CMeshManager::GetInstance();
	CTextureManager  * TextureManager    = CTextureManager::GetInstance();
	CTextureManager  * hudTextureManager = CTextureManager::GetHUDInstance();
    MeshManager->SetMesh("../waveFrontObj/cube_uv.obj");
    MeshManager->SetMesh("../waveFrontObj/sphere.obj");
    MeshManager->SetMesh("../waveFrontObj/cone.obj");
    MeshManager->SetMesh("../waveFrontObj/monkey.obj");
	MeshManager->SetMesh("../waveFrontObj/big_plain.obj");
    CEngine* GLVM = CEngine::GetInstance();	
    CTexture Texture_0{ .iWidth_ = 128, .iHeight_ = 96,
		.dat_length_ = chelik_dat_len, .u_iData_ = chelik_dat };
    CTexture Texture_1{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = witch_dat_len, .u_iData_ = witch_dat };
    CTexture Texture_2{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = gray_dat_len, .u_iData_ = gray_dat };
	CTexture Texture_3{ .iWidth_ = 500, .iHeight_ = 500,
		.dat_length_ = container2_dat_len, .u_iData_ = container2_dat };
	CTexture Texture_4{ .iWidth_ = 500, .iHeight_ = 500,
		.dat_length_ = container2_specular_dat_len, .u_iData_ = container2_specular_dat };
    
    std::vector<CTexture> TextureVector{ Texture_0, Texture_1, Texture_2, Texture_3, Texture_4 };
	
    TextureManager->SetTextureVector(TextureVector);

    CTexture hudTexture_0{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = Crosshair_dat_len, .u_iData_ = Crosshair_dat };
    // CTexture hudTexture_1;
    // hudTexture_1.iWidth_ = 32;
    // hudTexture_1.iHeight_ = 32;
    // hudTexture_1.u_iData_ = witch_dat;
    // hudTexture_1.dat_length_ = witch_dat_len;
    
    std::vector<CTexture> hudTextureVector;
    hudTextureVector.push_back(hudTexture_0);
    //    hudTextureVector.push_back(hudTexture_1);

    hudTextureManager->SetTextureVector(hudTextureVector);
	
    Entity uiPlayer = EntityManager->CreateEntity();
    ComponentManager->CreateComponent<vertex, movement, collider, animation, camera, transform, rigidBody, event>(uiPlayer);
	transform& transformPlayer = ComponentManager->GetComponent<transform>(uiPlayer);
    camera   & viewPlayer      = ComponentManager->GetComponent<camera>(uiPlayer);
    collider & colliderPlayer  = ComponentManager->GetComponent<collider>(uiPlayer);
    vertex   & vertexPlayer    = ComponentManager->GetComponent<vertex>(uiPlayer);
    vertexPlayer.vkVertexId_        = 0;
    colliderPlayer.bPush_Collission = true;
	transformPlayer  = { .tPosition = { 3.0f, 0.7f, 0.0f }, .fScale = 0.1f };
	viewPlayer       = { .Front_Camera = { 0.0f, 0.0f, -1.0f }, .Up_Camera = { 0.0f, 1.0f, 0.0f } };

	Entity plain0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<material, vertex, transform>(plain0);
	material & materialPlain0  = ComponentManager->GetComponent<material>(plain0);
	transform& transformPlain0 = ComponentManager->GetComponent<transform>(plain0);
    vertex   & vertexPlain0    = ComponentManager->GetComponent<vertex>(plain0);
	materialPlain0  = { .diffuseTextureID_ = 3, .specularTextureID_ = 4, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
	transformPlain0 = { .tPosition = { 0.0f, -0.5f, 0.0f }, .fScale = 1.0f };
    TextureManager->BindTexture(plain0, materialPlain0.diffuseTextureID_);
    vertexPlain0.vkVertexId_ = 4;
    
	Entity uiWitch = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<material, vertex, collider, transform>(uiWitch);
	material & materialWitch  = ComponentManager->GetComponent<material>(uiWitch);
	transform& transformWitch = ComponentManager->GetComponent<transform>(uiWitch);
    vertex   & vertexWitch    = ComponentManager->GetComponent<vertex>(uiWitch);
	materialWitch  = { .diffuseTextureID_ = 3, .specularTextureID_ = 4, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
	transformWitch = { .tPosition = { 0.0f, 0.0f, 0.0f }, .fScale = 1.0f };
    TextureManager->BindTexture(uiWitch, materialWitch.diffuseTextureID_);
    vertexWitch.vkVertexId_ = 0;
	
    Entity uiWitch2 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<material, vertex, collider, transform>(uiWitch2);
	material & materialWitch2  = ComponentManager->GetComponent<material>(uiWitch2);
	vertex   & vertexWitch2    = ComponentManager->GetComponent<vertex>(uiWitch2);
	transform& transformWitch2 = ComponentManager->GetComponent<transform>(uiWitch2);
	materialWitch2 = { .diffuseTextureID_ = 3, .specularTextureID_ = 4, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
	transformWitch2 = { .tPosition = { 3.7f, 0.0f, 0.0f }, .fScale = 1.0f };
	vertexWitch2.vkVertexId_ = 0;
    TextureManager->BindTexture(uiWitch2, materialWitch2.diffuseTextureID_);

	Entity uiWitch3 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<material, vertex, collider, transform>(uiWitch3);
	material & materialWitch3  = ComponentManager->GetComponent<material>(uiWitch3);
	vertex   & vertexWitch3    = ComponentManager->GetComponent<vertex>(uiWitch3);
	transform& transformWitch3 = ComponentManager->GetComponent<transform>(uiWitch3);
	materialWitch3 = { .diffuseTextureID_ = 3, .specularTextureID_ = 4, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
	transformWitch3 = { .tPosition = { 2.7f, 2.5f, 3.0f }, .fScale = 1.0f };
	vertexWitch3.vkVertexId_ = 1;
    TextureManager->BindTexture(uiWitch3, materialWitch3.diffuseTextureID_);
	
    // Entity u_iHud1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<vertex, material, collider, transform>(u_iHud1);
	// material & TextureHud1   = ComponentManager->GetComponent<material>(u_iHud1);
    // vertex   & VertexHud1    = ComponentManager->GetComponent<vertex>(u_iHud1);
	// transform& TransformHud1 = ComponentManager->GetComponent<transform>(u_iHud1);
    // VertexHud1.vkVertexId_ = 0;
	// TextureHud1 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
    // hudTextureManager->BindTexture(u_iHud1, TextureHud1.diffuseTextureID_);
	// TransformHud1 = { .tPosition = { 0.0f, 0.0f, 0.0f }, .fScale = 0.1f ,.hud = true };

	Entity directionalLight0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<vertex, material, SDirectionalLightComponent, transform>(directionalLight0);
	material                  & materialDirectionalLight0  = ComponentManager->GetComponent<material>(directionalLight0);
	SDirectionalLightComponent& directionalLightComponent0          = ComponentManager->GetComponent<SDirectionalLightComponent>(directionalLight0);
	transform                 & transformDirectionalLight0 = ComponentManager->GetComponent<transform>(directionalLight0);
	vertex                    & vertexDirectionalLight0    = ComponentManager->GetComponent<vertex>(directionalLight0);
	materialDirectionalLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	TextureManager->BindTexture(directionalLight0, materialDirectionalLight0.diffuseTextureID_);
	vertexDirectionalLight0.vkVertexId_ = 0;
	directionalLightComponent0 = {.direction = {0.0f, -3.0f, 0.0f}, .ambient = {0.2f, 0.2f, 0.2f},
		.diffuse = {0.5f, 0.5f, 0.5f}, .specular = {1.0f, 1.0f, 1.0f}};
	transformDirectionalLight0 = { .tPosition = { 3.0f, 5.0f, 1.0f }, .fScale = 0.3f };

	// Entity pointLight0 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<vertex, material, SPointLightComponent, transform>(pointLight0);
	// material            & texturePointLight0   = ComponentManager->GetComponent<material>(pointLight0);
	// SPointLightComponent& pointLightComponent0 = ComponentManager->GetComponent<SPointLightComponent>(pointLight0);
	// transform           & transformPointLight0 = ComponentManager->GetComponent<transform>(pointLight0);
	// vertex              & vertexPointLight0    = ComponentManager->GetComponent<vertex>(pointLight0);
	// texturePointLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	// vertexPointLight0.vkVertexId_ = 0;
	// TextureManager->BindTexture(pointLight0, texturePointLight0.diffuseTextureID_);
	// pointLightComponent0 = { .position = { 1.0f, 0.3f, 1.7f }, .ambient = { 0.2f, 0.2f, 0.2f },
	// 	.diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f,
	// 	.linear = 0.09f, .quadratic = 0.1f };
	// transformPointLight0 = { .tPosition = { 1.0f, 0.3f, 2.7f }, .fScale = 0.3f };

	// Entity pointLight1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<vertex, material, SPointLightComponent, transform>(pointLight1);
	// material& texturePointLight1 = ComponentManager->GetComponent<material>(pointLight1);
	// SPointLightComponent& pointLightComponent1 = ComponentManager->GetComponent<SPointLightComponent>(pointLight1);
	// transform& transformPointLight1 = ComponentManager->GetComponent<transform>(pointLight1);
	// vertex& vertexPointLight1 = ComponentManager->GetComponent<vertex>(pointLight1);
	// texturePointLight1 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	// TextureManager->BindTexture(pointLight1, texturePointLight1.diffuseTextureID_);
	// vertexPointLight1.vkVertexId_ = 0;
	// pointLightComponent1 = { .position = { 1.2f, 1.3f, 1.5f }, .ambient = { 0.2f, 0.2f, 0.2f },
	// 	.diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f,
	// 	.linear = 0.09f, .quadratic = 0.032f };
	// transformPointLight1 = { .tPosition = { 1.2f, 1.3f, 1.5f }, .fScale = 0.1f };

// 	Entity spotLight0 = EntityManager->CreateEntity();
// 	ComponentManager->CreateComponent<vertex, material, spotLight, transform>(spotLight0);
// 	material & textureSpotLight0   = ComponentManager->GetComponent<material>(spotLight0);
// 	spotLight& spotLightComponent0 = ComponentManager->GetComponent<spotLight>(spotLight0);
// 	transform& transformSpotLight0 = ComponentManager->GetComponent<transform>(spotLight0);
// 	vertex   & vertexSpotLight0    = ComponentManager->GetComponent<vertex>(spotLight0);
// 	textureSpotLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
// 	TextureManager->BindTexture(spotLight0, textureSpotLight0.diffuseTextureID_);
// 	vertexSpotLight0.vkVertexId_ = 0;
// //	spotLightComponent0.position     =  TransformPlayer.tPosition;
// //	spotLightComponent0.direction    =  TransformPlayer.tForward;
// 	spotLightComponent0 = { .cutOff = 12.5f, .outerCutOff = 17.5f,
// 		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.5f, 0.5f, 0.5f },
// 		.specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f,
// 		.linear = 0.09f, .quadratic = 0.032f };
// 	transformSpotLight0 = { .tPosition = transformPlayer.tPosition, .fScale = 0.3f };
    
    ///< Game rendering loop
	GLVM->GameLoop();
	GLVM->GameKill();

    return 0;
}
