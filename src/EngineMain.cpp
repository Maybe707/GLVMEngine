#include "ComponentManager.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
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
	namespace cm = GLVM::ecs::components;
	
	ecs::CEntityManager   * EntityManager     = ecs::CEntityManager::GetInstance();
	ecs::CComponentManager* ComponentManager  = ecs::CComponentManager::GetInstance();
	core::CMeshManager    * MeshManager       = core::CMeshManager::GetInstance();
	ecs::CTextureManager  * TextureManager    = ecs::CTextureManager::GetInstance();
	ecs::CTextureManager  * hudTextureManager = ecs::CTextureManager::GetHUDInstance();
    MeshManager->SetMesh("../waveFrontObj/cube_uv.obj");
    MeshManager->SetMesh("../waveFrontObj/sphere.obj");
    MeshManager->SetMesh("../waveFrontObj/cone.obj");
    MeshManager->SetMesh("../waveFrontObj/monkey.obj");
	MeshManager->SetMesh("../waveFrontObj/plain5.obj");
	MeshManager->SetMesh("../waveFrontObj/chrismas_tree.obj");
//	MeshManager->SetMesh("../waveFrontObj/shama_final.obj");
	core::CEngine* GLVM = core::CEngine::GetInstance();	
	ecs::CTexture Texture_0{ .iWidth_ = 128, .iHeight_ = 96,
		.dat_length_ = chelik_dat_len, .u_iData_ = chelik_dat };
	ecs::CTexture Texture_1{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = witch_dat_len, .u_iData_ = witch_dat };
	ecs::CTexture Texture_2{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = gray_dat_len, .u_iData_ = gray_dat };
	ecs::CTexture Texture_3{ .iWidth_ = 500, .iHeight_ = 500,
		.dat_length_ = container2_dat_len, .u_iData_ = container2_dat };
	ecs::CTexture Texture_4{ .iWidth_ = 500, .iHeight_ = 500,
		.dat_length_ = container2_specular_dat_len, .u_iData_ = container2_specular_dat };
    
    std::vector<ecs::CTexture> TextureVector{ Texture_0, Texture_1, Texture_2, Texture_3, Texture_4 };
	
    TextureManager->SetTextureVector(TextureVector);
	
	ecs::CTexture hudTexture_0{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = Crosshair_dat_len, .u_iData_ = Crosshair_dat };
    // CTexture hudTexture_1;
    // hudTexture_1.iWidth_ = 32;
    // hudTexture_1.iHeight_ = 32;
    // hudTexture_1.u_iData_ = witch_dat;
    // hudTexture_1.dat_length_ = witch_dat_len;
    
    std::vector<ecs::CTexture> hudTextureVector;
    hudTextureVector.push_back(hudTexture_0);
    //    hudTextureVector.push_back(hudTexture_1);

    hudTextureManager->SetTextureVector(hudTextureVector);
	
    Entity uiPlayer = EntityManager->CreateEntity();
    ComponentManager->CreateComponent<cm::vertex, cm::move, cm::collider, cm::animation, cm::beholder, cm::transform, cm::rigidBody, cm::event>(uiPlayer);
	cm::transform& transformPlayer = ComponentManager->GetComponent<cm::transform>(uiPlayer);
    cm::beholder   & viewPlayer      = ComponentManager->GetComponent<cm::beholder>(uiPlayer);
    cm::collider & colliderPlayer  = ComponentManager->GetComponent<cm::collider>(uiPlayer);
    cm::vertex   & vertexPlayer    = ComponentManager->GetComponent<cm::vertex>(uiPlayer);
    vertexPlayer.vkVertexId_        = 0;
    colliderPlayer.bPush_Collission = true;
	transformPlayer  = { .tPosition = { 0.0f, 1.0f, 0.0f }, .fScale = 0.1f };
	viewPlayer       = { .Front_Camera = { 0.0f, 0.0f, -1.0f }, .Up_Camera = { 0.0f, 1.0f, 0.0f } };

	Entity plain0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::vertex, cm::transform>(plain0);
	cm::material & materialPlain0  = ComponentManager->GetComponent<cm::material>(plain0);
	cm::transform& transformPlain0 = ComponentManager->GetComponent<cm::transform>(plain0);
    cm::vertex   & vertexPlain0    = ComponentManager->GetComponent<cm::vertex>(plain0);
	materialPlain0  = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
	transformPlain0 = { .tPosition = { 5.5f, 6.5f, 0.0f }, .fScale = 1.2f };
    TextureManager->BindTexture(plain0, materialPlain0.diffuseTextureID_);
    vertexPlain0.vkVertexId_ = 4;
    
	Entity uiWitch = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::vertex, cm::collider, cm::transform>(uiWitch);
	cm::material & materialWitch  = ComponentManager->GetComponent<cm::material>(uiWitch);
	cm::transform& transformWitch = ComponentManager->GetComponent<cm::transform>(uiWitch);
    cm::vertex   & vertexWitch    = ComponentManager->GetComponent<cm::vertex>(uiWitch);
	materialWitch  = { .diffuseTextureID_ = 2, .specularTextureID_ = 2, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
	transformWitch = { .tPosition = { 0.5f, 1.0f, 0.5f }, .fScale = 0.5f };
    TextureManager->BindTexture(uiWitch, materialWitch.diffuseTextureID_);
    vertexWitch.vkVertexId_ = 0;
	
    Entity uiWitch2 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::vertex, cm::collider, cm::transform>(uiWitch2);
	cm::material & materialWitch2  = ComponentManager->GetComponent<cm::material>(uiWitch2);
	cm::vertex   & vertexWitch2    = ComponentManager->GetComponent<cm::vertex>(uiWitch2);
	cm::transform& transformWitch2 = ComponentManager->GetComponent<cm::transform>(uiWitch2);
	materialWitch2 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
	transformWitch2 = { .tPosition = { 5.5f, 0.5f, 1.5f }, .fScale = 1.0f };
	vertexWitch2.vkVertexId_ = 5;
    TextureManager->BindTexture(uiWitch2, materialWitch2.diffuseTextureID_);

	Entity uiWitch3 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::material, cm::vertex, cm::collider, cm::transform>(uiWitch3);
	cm::material & materialWitch3  = ComponentManager->GetComponent<cm::material>(uiWitch3);
	cm::vertex   & vertexWitch3    = ComponentManager->GetComponent<cm::vertex>(uiWitch3);
	cm::transform& transformWitch3 = ComponentManager->GetComponent<cm::transform>(uiWitch3);
	materialWitch3 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1, .ambient = { 0.05f, 0.05f, 0.0f },
		.shininess = 128.0f * 0.078125f };
	transformWitch3 = { .tPosition = { 3.7f, 2.5f, 3.0f }, .fScale = 1.0f };
	vertexWitch3.vkVertexId_ = 1;
    TextureManager->BindTexture(uiWitch3, materialWitch3.diffuseTextureID_);
	
    // Entity u_iHud1 = EntityManager->CreateEntity();
	// ComponentManager->CreateComponent<cm::vertex, cm::material, cm::collider, cm::transform>(u_iHud1);
	// cm::material & TextureHud1   = ComponentManager->GetComponent<cm::material>(u_iHud1);
    // cm::vertex   & VertexHud1    = ComponentManager->GetComponent<cm::vertex>(u_iHud1);
	// cm::transform& TransformHud1 = ComponentManager->GetComponent<cm::transform>(u_iHud1);
    // VertexHud1.vkVertexId_ = 0;
	// TextureHud1 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
    // hudTextureManager->BindTexture(u_iHud1, TextureHud1.diffuseTextureID_);
	// TransformHud1 = { .tPosition = { 0.0f, 0.0f, 0.0f }, .fScale = 0.1f ,.hud = true };

	Entity directionalLight0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::directionalLight, cm::transform>(directionalLight0);
	cm::material                  & materialDirectionalLight0  = ComponentManager->GetComponent<cm::material>(directionalLight0);
	cm::directionalLight& directionalLightComponent0          = ComponentManager->GetComponent<cm::directionalLight>(directionalLight0);
	cm::transform                 & transformDirectionalLight0 = ComponentManager->GetComponent<cm::transform>(directionalLight0);
	cm::vertex                    & vertexDirectionalLight0    = ComponentManager->GetComponent<cm::vertex>(directionalLight0);
	materialDirectionalLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	TextureManager->BindTexture(directionalLight0, materialDirectionalLight0.diffuseTextureID_);
	vertexDirectionalLight0.vkVertexId_ = 0;
	directionalLightComponent0 = { .position = { 2.0f, 5.0f, -8.0f }, .direction = { 0.0f, 1.5f, -1.0f},
		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = {0.5f, 0.5f, 0.5f}, .specular = {1.0f, 1.0f, 1.0f}};
	transformDirectionalLight0 = { .tPosition = { 2.0f, 5.0f, -8.0f }, .fScale = 0.3f };

	Entity directionalLight1 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::directionalLight, cm::transform>(directionalLight1);
	cm::material                  & materialDirectionalLight1  = ComponentManager->GetComponent<cm::material>(directionalLight1);
	cm::directionalLight          & directionalLightComponent1 = ComponentManager->GetComponent<cm::directionalLight>(directionalLight1);
	cm::transform                 & transformDirectionalLight1 = ComponentManager->GetComponent<cm::transform>(directionalLight1);
	cm::vertex                    & vertexDirectionalLight1    = ComponentManager->GetComponent<cm::vertex>(directionalLight1);
	materialDirectionalLight1 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	TextureManager->BindTexture(directionalLight1, materialDirectionalLight1.diffuseTextureID_);
	vertexDirectionalLight1.vkVertexId_ = 0;
	directionalLightComponent1 = { .position = { 20.0f, 5.0f, 5.0f }, .direction = { 0.0f, 1.5f, -1.0f},
		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = {0.5f, 0.5f, 0.5f}, .specular = {0.0f, 0.0f, 0.0f}};
	transformDirectionalLight1 = { .tPosition = { 20.0f, 5.0f, 5.0f }, .fScale = 0.3f };
	
	Entity pointLight0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::pointLight, cm::transform>(pointLight0);
	cm::material            & texturePointLight0   = ComponentManager->GetComponent<cm::material>(pointLight0);
	cm::pointLight& pointLightComponent0 = ComponentManager->GetComponent<cm::pointLight>(pointLight0);
	cm::transform           & transformPointLight0 = ComponentManager->GetComponent<cm::transform>(pointLight0);
	cm::vertex              & vertexPointLight0    = ComponentManager->GetComponent<cm::vertex>(pointLight0);
	texturePointLight0 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	vertexPointLight0.vkVertexId_ = 0;
	TextureManager->BindTexture(pointLight0, texturePointLight0.diffuseTextureID_);
	pointLightComponent0 = { .position = { 0.0f, 3.3f, 2.7f }, .ambient = { 0.2f, 0.2f, 0.2f },
		.diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f,
		.linear = 0.09f, .quadratic = 0.1f };
	transformPointLight0 = { .tPosition = { 0.0f, 3.3f, 2.7f }, .fScale = 0.3f };

	Entity pointLight1 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::pointLight, cm::transform>(pointLight1);
	cm::material& texturePointLight1 = ComponentManager->GetComponent<cm::material>(pointLight1);
	cm::pointLight& pointLightComponent1 = ComponentManager->GetComponent<cm::pointLight>(pointLight1);
	cm::transform& transformPointLight1 = ComponentManager->GetComponent<cm::transform>(pointLight1);
	cm::vertex& vertexPointLight1 = ComponentManager->GetComponent<cm::vertex>(pointLight1);
	texturePointLight1 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	TextureManager->BindTexture(pointLight1, texturePointLight1.diffuseTextureID_);
	vertexPointLight1.vkVertexId_ = 0;
	pointLightComponent1 = { .position = { 5.2f, 3.3f, 3.5f }, .ambient = { 0.2f, 0.2f, 0.2f },
		.diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f,
		.linear = 0.09f, .quadratic = 0.032f };
	transformPointLight1 = { .tPosition = { 5.2f, 3.3f, 3.5f }, .fScale = 0.3f };

	Entity pointLight2 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::pointLight, cm::transform>(pointLight2);
	cm::material& texturePointLight2 = ComponentManager->GetComponent<cm::material>(pointLight2);
	cm::pointLight& pointLightComponent2 = ComponentManager->GetComponent<cm::pointLight>(pointLight2);
	cm::transform& transformPointLight2 = ComponentManager->GetComponent<cm::transform>(pointLight2);
	cm::vertex& vertexPointLight2 = ComponentManager->GetComponent<cm::vertex>(pointLight2);
	texturePointLight2 = { .diffuseTextureID_ = 1, .specularTextureID_ = 1 };
	TextureManager->BindTexture(pointLight2, texturePointLight2.diffuseTextureID_);
	vertexPointLight2.vkVertexId_ = 0;
	pointLightComponent2 = { .position = { 9.2f, 3.3f, 7.5f }, .ambient = { 0.2f, 0.2f, 0.2f },
		.diffuse = { 0.5f, 0.5f, 0.5f }, .specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f,
		.linear = 0.09f, .quadratic = 0.032f };
	transformPointLight2 = { .tPosition = { 9.2f, 3.3f, 7.5f }, .fScale = 0.3f };

	
	Entity spotLight0 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::spotLight, cm::transform>(spotLight0);
	cm::material & textureSpotLight0   = ComponentManager->GetComponent<cm::material>(spotLight0);
	cm::spotLight& spotLightComponent0 = ComponentManager->GetComponent<cm::spotLight>(spotLight0);
	cm::transform& transformSpotLight0 = ComponentManager->GetComponent<cm::transform>(spotLight0);
	cm::vertex   & vertexSpotLight0    = ComponentManager->GetComponent<cm::vertex>(spotLight0);
	textureSpotLight0 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
	TextureManager->BindTexture(spotLight0, textureSpotLight0.diffuseTextureID_);
	vertexSpotLight0.vkVertexId_ = 0;
//	spotLightComponent0.position     =  TransformPlayer.tPosition;
//	spotLightComponent0.direction    =  TransformPlayer.tForward;
	spotLightComponent0 = { .position = { -3.0f, 3.0f, 1.0f }, .direction = { 3.0f, -1.5f, -0.5f },
		.cutOff = 12.5f, .outerCutOff = 17.5f,
		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.5f, 0.5f, 0.5f },
		.specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f,
		.linear = 0.09f, .quadratic = 0.032f };
//	transformSpotLight0 = { .tPosition = transformPlayer.tPosition, .fScale = 0.3f };
	transformSpotLight0 = { .tPosition = { -3.0f, 3.0f, 1.0f }, .fScale = 0.3f };

	Entity spotLight1 = EntityManager->CreateEntity();
	ComponentManager->CreateComponent<cm::vertex, cm::material, cm::spotLight, cm::transform>(spotLight1);
	cm::material & textureSpotLight1   = ComponentManager->GetComponent<cm::material>(spotLight1);
	cm::spotLight& spotLightComponent1 = ComponentManager->GetComponent<cm::spotLight>(spotLight1);
	cm::transform& transformSpotLight1 = ComponentManager->GetComponent<cm::transform>(spotLight1);
	cm::vertex   & vertexSpotLight1    = ComponentManager->GetComponent<cm::vertex>(spotLight1);
	textureSpotLight1 = { .diffuseTextureID_ = 0, .specularTextureID_ = 0 };
	TextureManager->BindTexture(spotLight1, textureSpotLight1.diffuseTextureID_);
	vertexSpotLight1.vkVertexId_ = 0;
//	spotLightComponent0.position     =  TransformPlayer.tPosition;
//	spotLightComponent0.direction    =  TransformPlayer.tForward;
	spotLightComponent1 = { .position = { 1.0f, 3.0f, 4.0f }, .direction = { -0.5f, -1.5f, -3.5f },
		.cutOff = 12.5f, .outerCutOff = 17.5f,
		.ambient = { 0.2f, 0.2f, 0.2f }, .diffuse = { 0.5f, 0.5f, 0.5f },
		.specular = { 1.0f, 1.0f, 1.0f }, .constant = 1.0f,
		.linear = 0.09f, .quadratic = 0.032f };
//	transformSpotLight0 = { .tPosition = transformPlayer.tPosition, .fScale = 0.3f };
	transformSpotLight1 = { .tPosition = { 1.0f, 3.0f, 4.0f }, .fScale = 0.3f };
	
    ///< Game rendering loop
	GLVM->GameLoop();
	GLVM->GameKill();

    return 0;
}
