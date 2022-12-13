#include "Components/LightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Engine.hpp"
#include "MeshManager.hpp"
#include "SpritesData.hpp"
#include "TextureManager.hpp"
#include "Texture.hpp"
#include <math.h>

int main()
{
    GLVM::ECS::CEntityManager*    EntityManager    = GLVM::ECS::CEntityManager::GetInstance();
    GLVM::ECS::CComponentManager* ComponentManager = GLVM::ECS::CComponentManager::GetInstance();
    GLVM::Core::CMeshManager*     MeshManager      = GLVM::Core::CMeshManager::GetInstance();
	
    MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/cube_uv.obj");
    MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/sphere.obj");
    MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/cone.obj");
    MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/monkey.obj");
//    MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/xyz.obj");
//	MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/shaman.obj");
//	MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/shama_final.obj");
//	MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/F.obj");
//	MeshManager->SetMesh("/home/cyberdemon/cyberDemonCode/GLVMEngine/waveFrontObj/sphere_test.obj");
	
    GLVM::ECS::CTexture Texture_0{ .iWidth_ = 128, .iHeight_ = 96,
		.dat_length_ = chelik_dat_len, .u_iData_ = chelik_dat };
    GLVM::ECS::CTexture Texture_1{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = witch_dat_len, .u_iData_ = witch_dat };
    GLVM::ECS::CTexture Texture_2{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = gray_dat_len, .u_iData_ = gray_dat };
	GLVM::ECS::CTexture Texture_3{ .iWidth_ = 500, .iHeight_ = 500,
		.dat_length_ = container2_dat_len, .u_iData_ = container2_dat };
	GLVM::ECS::CTexture Texture_4{ .iWidth_ = 500, .iHeight_ = 500,
		.dat_length_ = container2_specular_dat_len, .u_iData_ = container2_specular_dat };
    
    std::vector<GLVM::ECS::CTexture> TextureVector;
    TextureVector.push_back(Texture_0);
    TextureVector.push_back(Texture_1);
	TextureVector.push_back(Texture_2);
	TextureVector.push_back(Texture_3);
	TextureVector.push_back(Texture_4);
	
    GLVM::ECS::CTextureManager* TextureSystem = GLVM::ECS::CTextureManager::GetInstance();
    TextureSystem->SetTextureVector(TextureVector);

    GLVM::ECS::CTexture hudTexture_0{ .iWidth_ = 32, .iHeight_ = 32,
		.dat_length_ = Crosshair_dat_len, .u_iData_ = Crosshair_dat };
    // GLVM::ECS::CTexture hudTexture_1;
    // hudTexture_1.iWidth_ = 32;
    // hudTexture_1.iHeight_ = 32;
    // hudTexture_1.u_iData_ = witch_dat;
    // hudTexture_1.dat_length_ = witch_dat_len;
    
    std::vector<GLVM::ECS::CTexture> hudTextureVector;
    hudTextureVector.push_back(hudTexture_0);
    //    hudTextureVector.push_back(hudTexture_1);
    
    GLVM::ECS::CTextureManager* hudTextureManager = GLVM::ECS::CTextureManager::GetHUDInstance();
    hudTextureManager->SetTextureVector(hudTextureVector);
    
    Entity u_iPlayer;
    EntityManager->CreateEntity(u_iPlayer);
    ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::SMoveComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::SAnimationMoveComponent, GLVM::ECS::CViewComponent, GLVM::ECS::STransformComponent, GLVM::ECS::CRigidBodyComponent, GLVM::ECS::SEventComponent>(u_iPlayer);
	// TexturePlayer.iWidth_ = 128; 
	// TexturePlayer.iHeight_ = 96;
	// TexturePlayer.u_iData_ = chelik_dat;
    // TexturePlayer.dat_length_ = chelik_dat_len;
//	GLVM->LoadTextureData(TexturePlayer);
	GLVM::ECS::STransformComponent& TransformPlayer = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iPlayer);
    GLVM::ECS::CViewComponent& ViewPlayer = ComponentManager->GetComponent<GLVM::ECS::CViewComponent>(u_iPlayer);
    GLVM::ECS::CRigidBodyComponent& GravityPlayer = ComponentManager->GetComponent<GLVM::ECS::CRigidBodyComponent>(u_iPlayer);
    GLVM::ECS::CColliderComponent& ColliderPlayer = ComponentManager->GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
    GLVM::ECS::SVertexComponent& VertexPlayer = ComponentManager->GetComponent<GLVM::ECS::SVertexComponent>(u_iPlayer);
	// GLVM::Core::SMaterialComponent& TexturePlayer = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(u_iPlayer);
    // TexturePlayer.id_ = 2;
    // TextureSystem->BindTexture(u_iPlayer, TexturePlayer.id_);
    VertexPlayer.vkVertexId_ = 0;
    ColliderPlayer.bPush_Collission = true;
    TransformPlayer.fScale = 0.1f;
    TransformPlayer.tPosition[0] = 0.0f;
	TransformPlayer.tPosition[1] = 0.3f;
	TransformPlayer.tPosition[2] = -1.0f;
    ViewPlayer.Front_Camera[0] = 0.0f;
    ViewPlayer.Front_Camera[1] = 0.0f;
    ViewPlayer.Front_Camera[2] = -1.0f;
    ViewPlayer.Up_Camera[0] = 0.0f;
    ViewPlayer.Up_Camera[1] = 1.0f;
    ViewPlayer.Up_Camera[2] = 0.0f;
    // GravityPlayer.tGravity_[0] = 0.0f;
    // GravityPlayer.tGravity_[1] = 0.0f;
    // GravityPlayer.tGravity_[2] = 0.0f;
    // ColliderPlayer.aColliders_Container_[0].bGround_Collision_ = false;
    // ColliderPlayer.aColliders_Container_[0].bWall_Collision_ = false;
    
	Entity u_iWitch;
	EntityManager->CreateEntity(u_iWitch);
	ComponentManager->CreateComponent<GLVM::Core::SMaterialComponent, GLVM::ECS::SVertexComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch);
	GLVM::Core::SMaterialComponent& TextureWitch = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(u_iWitch);
	GLVM::Core::SMaterialComponent& materialWitch = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(u_iWitch);
	materialWitch.ambient[0] = 0.25f;
	materialWitch.ambient[1] = 0.25f;
	materialWitch.ambient[2] = 0.25f;
	// materialWitch.diffuse[0] = 0.4f;
	// materialWitch.diffuse[1] = 0.4f;
	// materialWitch.diffuse[2] = 0.4f;
	materialWitch.specular[0] = 0.774597f;
	materialWitch.specular[1] = 0.774597f;
	materialWitch.specular[2] = 0.774597f;
	materialWitch.shininess = 128.0f * 0.6f;
    TextureWitch.id_ = 3;
    TextureSystem->BindTexture(u_iWitch, TextureWitch.id_);
	// TextureWitch.iWidth_ = 32;
	// TextureWitch.iHeight_ = 32;
	// TextureWitch.u_iData_ = witch_dat;
    // TextureWitch.dat_length_ = witch_dat_len;
//	GLVM->LoadTextureData(TextureWitch);
	GLVM::ECS::STransformComponent& TransformWitch = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iWitch);
//    GLVM::ECS::CColliderComponent& ColliderWitch1 = ComponentManager.GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
    GLVM::ECS::SVertexComponent& VertexWitch = ComponentManager->GetComponent<GLVM::ECS::SVertexComponent>(u_iWitch);
    VertexWitch.vkVertexId_ = 0;
    TransformWitch.tPosition[0] = 0.3f;
    TransformWitch.tPosition[1] = 0.3f;
    TransformWitch.tPosition[2] = 0.3f;
    TransformWitch.fScale = 1.0f;
    // ColliderWitch1.aColliders_Container_[0].bGround_Collision_ = false;
    // ColliderWitch1.aColliders_Container_[0].bWall_Collision_ = false;

    Entity u_iWitch2;
	EntityManager->CreateEntity(u_iWitch2);
	ComponentManager->CreateComponent<GLVM::Core::SMaterialComponent, GLVM::ECS::SVertexComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch2);
	GLVM::Core::SMaterialComponent& TextureWitch2 = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(u_iWitch2);
    TextureWitch2.id_ = 4;
    GLVM::ECS::SVertexComponent& VertexWitch2 = ComponentManager->GetComponent<GLVM::ECS::SVertexComponent>(u_iWitch2);
    VertexWitch2.vkVertexId_ = 0;
    TextureSystem->BindTexture(u_iWitch2, TextureWitch2.id_);
	// TextureWitch2.iWidth_ = 32;
	// TextureWitch2.iHeight_ = 32;
	// TextureWitch2.u_iData_ = witch_dat;
    // TextureWitch2.dat_length_ = witch_dat_len;
//	GLVM->LoadTextureData(TextureWitch2);
	GLVM::Core::SMaterialComponent& materialWitch2 = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(u_iWitch2);
 	materialWitch2.ambient[0] = 0.05f;
	materialWitch2.ambient[1] = 0.05f;
	materialWitch2.ambient[2] = 0.0f;
	// materialWitch2.diffuse[0] = 0.5f;
	// materialWitch2.diffuse[1] = 0.5f;
	// materialWitch2.diffuse[2] = 0.4f;
	materialWitch2.specular[0] = 0.7f;
	materialWitch2.specular[1] = 0.7f;
	materialWitch2.specular[2] = 0.04f;
	materialWitch2.shininess = 128.0f * 0.078125f;
	GLVM::ECS::STransformComponent& TransformWitch2 = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iWitch2);
//    GLVM::ECS::CColliderComponent& ColliderWitch2 = ComponentManager.GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
    TransformWitch2.tPosition[0] = 2.5f;
    TransformWitch2.tPosition[1] = 0.3f;
    TransformWitch2.tPosition[2] = 0.0f;
    TransformWitch2.fScale = 1.0f;
//    TransformWitch2.hud = true;
//    std::cout << "GROUND: " <<  ColliderWitch2.aColliders_Container_[0].bGround_Collision_ << std::endl;
//    std::cout << "WALL: " << ColliderWitch2.aColliders_Container_[0].bWall_Collision_ << std::endl;

//     Entity u_iWitch3;
// 	EntityManager->CreateEntity(u_iWitch3);
// 	ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::Core::SMaterialComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch3);
// 	GLVM::Core::SMaterialComponent& TextureWitch3 = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(u_iWitch3);
//     TextureWitch3.id_ = 0;
//     hudTextureManager->BindTexture(u_iWitch3, TextureWitch3.id_);
// 	// TextureWitch2.iWidth_ = 32;
// 	// TextureWitch2.iHeight_ = 32;
// 	// TextureWitch2.u_iData_ = witch_dat;
//     // TextureWitch2.dat_length_ = witch_dat_len;
// //	GLVM->LoadTextureData(TextureWitch2);
// 	GLVM::ECS::STransformComponent& TransformWitch3 = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iWitch3);
// //    GLVM::ECS::CColliderComponent& ColliderWitch2 = ComponentManager.GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
//     TransformWitch3.tPosition[0] = 0.0f;
//     TransformWitch3.tPosition[1] = 0.0f;
//     TransformWitch3.tPosition[2] = 0.5f;
//     TransformWitch3.fScale = 0.1f;
// //    TransformWitch2.hud = true;
// //    std::cout << "GROUND: " <<  ColliderWitch2.aColliders_Container_[0].bGround_Collision_ << std::endl;
// //    std::cout << "WALL: " << ColliderWitch2.aColliders_Container_[0].bWall_Collision_ << std::endl;

    Entity u_iHud1;
	EntityManager->CreateEntity(u_iHud1);
	ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::Core::SMaterialComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iHud1);
	GLVM::Core::SMaterialComponent& TextureHud1 = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(u_iHud1);
    GLVM::ECS::SVertexComponent& VertexHud1 = ComponentManager->GetComponent<GLVM::ECS::SVertexComponent>(u_iHud1);
    VertexHud1.vkVertexId_ = 0;
    TextureHud1.id_ = 0;
    hudTextureManager->BindTexture(u_iHud1, TextureHud1.id_);
	// TextureWitch2.iWidth_ = 32;
	// TextureWitch2.iHeight_ = 32;
	// TextureWitch2.u_iData_ = witch_dat;
    // TextureWitch2.dat_length_ = witch_dat_len;
//	GLVM->LoadTextureData(TextureWitch2);
	GLVM::ECS::STransformComponent& TransformHud1 = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iHud1);
//    GLVM::ECS::CColliderComponent& ColliderWitch2 = ComponentManager.GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
    TransformHud1.tPosition[0] = 0.0f;
    TransformHud1.tPosition[1] = 0.0f;
    TransformHud1.tPosition[2] = 0.3f;
    TransformHud1.fScale = 0.1f;
   TransformHud1.hud = true;
//    std::cout << "GROUND: " <<  ColliderWitch2.aColliders_Container_[0].bGround_Collision_ << std::endl;
//    std::cout << "WALL: " << ColliderWitch2.aColliders_Container_[0].bWall_Collision_ << std::endl;

	Entity light0;
	EntityManager->CreateEntity(light0);
	ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::Core::SMaterialComponent, GLVM::Core::SLightComponent, GLVM::ECS::STransformComponent>(light0);
	GLVM::Core::SMaterialComponent& textureLight0 = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(light0);
	GLVM::Core::SLightComponent& Light0 = ComponentManager->GetComponent<GLVM::Core::SLightComponent>(light0);
	GLVM::ECS::STransformComponent& transformLight0 = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(light0);
	GLVM::ECS::SVertexComponent& vertexLight0 = ComponentManager->GetComponent<GLVM::ECS::SVertexComponent>(light0);
	textureLight0.id_ = 2;
	TextureSystem->BindTexture(light0, textureLight0.id_);
	vertexLight0.vkVertexId_ = 0;
	Light0.ambient[0]            =  0.2f;
	Light0.ambient[1]            =  0.2f;
	Light0.ambient[2]            =  0.2f;
	Light0.diffuse[0]            =  0.5f;
	Light0.diffuse[1]            =  0.5f;
	Light0.diffuse[2]            =  0.5f;
	Light0.specular[0]           =  1.0f;
	Light0.specular[1]           =  1.0f;
	Light0.specular[2]           =  1.0f;
	Light0.position[0]           =  1.5f;
	Light0.position[1]           =  1.5f;
	Light0.position[2]           =  2.9f;
	transformLight0.tPosition[0] =  1.5f;
	transformLight0.tPosition[1] =  1.3f;
	transformLight0.tPosition[2] =  2.9f;
	transformLight0.fScale = 0.1f;
	
//     Entity u_iWitch3;
// 	EntityManager->CreateEntity(u_iWitch3);
// 	ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::Core::SMaterialComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch3);
// 	GLVM::Core::SMaterialComponent& TextureWitch3 = ComponentManager->GetComponent<GLVM::Core::SMaterialComponent>(u_iWitch3);
// 	TextureWitch3.iWidth_ = 32;
// 	TextureWitch3.iHeight_ = 32;
// 	TextureWitch3.u_iData_ = witch_dat;
//     TextureWitch3.dat_length_ = witch_dat_len;
// //	GLVM->LoadTextureData(TextureWitch3);
// 	GLVM::ECS::STransformComponent& TransformWitch3 = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iWitch3);
// //    GLVM::ECS::CColliderComponent& ColliderWitch2 = ComponentManager.GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
//     TransformWitch3.tPosition[0] = 0.7f;
//     TransformWitch3.tPosition[1] = 0.7f;
//     TransformWitch3.tPosition[2] = 0.7f;
//     TransformWitch3.fScale = 1.0f;
    
    // Entity u_iWitch3;
	// EntityManager.CreateEntity(u_iWitch3);
	// ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::Core::SMaterialComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch3);
	// GLVM::Core::SMaterialComponent& TextureWitch3 = ComponentManager.GetComponent<GLVM::Core::SMaterialComponent>(u_iWitch3);
	// TextureWitch3.iWidth_ = 32;
	// TextureWitch3.iHeight_ = 32;
	// TextureWitch3.u_iData_ = witch_dat;
	// GLVM.LoadTextureData(TextureWitch3);
	// GLVM::ECS::STransformComponent& TransformWitch3 = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch3);
    // TransformWitch3.fScale = 1.0f;
    // TransformWitch3.tPosition[0] = 5.0f;
    // TransformWitch3.tPosition[1] = 1.0f;
    // TransformWitch3.tPosition[2] = 0.0f;

    GLVM::Core::CEngine* GLVM = GLVM::Core::CEngine::GetInstance();
    
    ///< Game rendering loop
	GLVM->GameLoop();
	GLVM->GameKill();

    return 0;
}
