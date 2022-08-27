#include "Components/TextureComponent.hpp"
#include "Engine.hpp"
#include "SpritesData.hpp"

int main()
{
    GLVM::ECS::CEntityManager*    EntityManager = GLVM::ECS::CEntityManager::GetInstance();
    GLVM::ECS::CComponentManager* ComponentManager = GLVM::ECS::CComponentManager::GetInstance();
    
	Entity u_iPlayer;
	EntityManager->CreateEntity(u_iPlayer);
    ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::SMoveComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::SAnimationMoveComponent, GLVM::ECS::CViewComponent, GLVM::ECS::STransformComponent, GLVM::ECS::CRigidBodyComponent, GLVM::ECS::SEventComponent>(u_iPlayer);
    GLVM::ECS::CTextureComponent& TexturePlayer = ComponentManager->GetComponent<GLVM::ECS::CTextureComponent>(u_iPlayer);
	TexturePlayer.iWidth_ = 128; 
	TexturePlayer.iHeight_ = 96;
	TexturePlayer.u_iData_ = chelik_dat;
    TexturePlayer.dat_length_ = chelik_dat_len;
//	GLVM->LoadTextureData(TexturePlayer);
	GLVM::ECS::STransformComponent& TransformPlayer = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iPlayer);
    GLVM::ECS::CViewComponent& ViewPlayer = ComponentManager->GetComponent<GLVM::ECS::CViewComponent>(u_iPlayer);
    GLVM::ECS::CRigidBodyComponent& GravityPlayer = ComponentManager->GetComponent<GLVM::ECS::CRigidBodyComponent>(u_iPlayer);
    GLVM::ECS::CColliderComponent& ColliderPlayer = ComponentManager->GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
    ColliderPlayer.bPush_Collission = true;
    TransformPlayer.fScale = 0.5f;
    TransformPlayer.tPosition[0] = 0.1f;
	TransformPlayer.tPosition[1] = 0.1f;
	TransformPlayer.tPosition[2] = 0.1f;
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
	ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch);
	GLVM::ECS::CTextureComponent& TextureWitch = ComponentManager->GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch);
	TextureWitch.iWidth_ = 32;
	TextureWitch.iHeight_ = 32;
	TextureWitch.u_iData_ = witch_dat;
    TextureWitch.dat_length_ = witch_dat_len;
//	GLVM->LoadTextureData(TextureWitch);
	GLVM::ECS::STransformComponent& TransformWitch = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iWitch);
//    GLVM::ECS::CColliderComponent& ColliderWitch1 = ComponentManager.GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
    TransformWitch.tPosition[0] = 0.3f;
    TransformWitch.tPosition[1] = 0.3f;
    TransformWitch.tPosition[2] = 0.3f;
    TransformWitch.fScale = 10.0f;
    // ColliderWitch1.aColliders_Container_[0].bGround_Collision_ = false;
    // ColliderWitch1.aColliders_Container_[0].bWall_Collision_ = false;

//     Entity u_iWitch2;
// 	EntityManager->CreateEntity(u_iWitch2);
// 	ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch2);
// 	GLVM::ECS::CTextureComponent& TextureWitch2 = ComponentManager->GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch2);
// 	TextureWitch2.iWidth_ = 32;
// 	TextureWitch2.iHeight_ = 32;
// 	TextureWitch2.u_iData_ = witch_dat;
//     TextureWitch2.dat_length_ = witch_dat_len;
// //	GLVM->LoadTextureData(TextureWitch2);
// 	GLVM::ECS::STransformComponent& TransformWitch2 = ComponentManager->GetComponent<GLVM::ECS::STransformComponent>(u_iWitch2);
// //    GLVM::ECS::CColliderComponent& ColliderWitch2 = ComponentManager.GetComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
//     TransformWitch2.tPosition[0] = 0.5f;
//     TransformWitch2.tPosition[1] = -1.5f;
//     TransformWitch2.tPosition[2] = -9.5f;
//     TransformWitch2.fScale = 10.0f;
// //    std::cout << "GROUND: " <<  ColliderWitch2.aColliders_Container_[0].bGround_Collision_ << std::endl;
// //    std::cout << "WALL: " << ColliderWitch2.aColliders_Container_[0].bWall_Collision_ << std::endl;

//     Entity u_iWitch3;
// 	EntityManager->CreateEntity(u_iWitch3);
// 	ComponentManager->CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch3);
// 	GLVM::ECS::CTextureComponent& TextureWitch3 = ComponentManager->GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch3);
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
	// ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch3);
	// GLVM::ECS::CTextureComponent& TextureWitch3 = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch3);
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
