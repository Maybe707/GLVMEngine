#include "Engine.hpp"
#include "SpritesData.hpp"

int main()
{
	GLVM::Core::CEngine GLVM;
	GLVM::ECS::CEntityManager EntityManager;
	GLVM::ECS::CComponentManager ComponentManager;
    
	Entity u_iPlayer = 0;
	EntityManager.CreateEntity(u_iPlayer);
    ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::SMoveComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::SAnimationMoveComponent, GLVM::ECS::CViewComponent, GLVM::ECS::STransformComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CGravityComponent, GLVM::ECS::SEventComponent>(u_iPlayer);
    GLVM::ECS::CTextureComponent& TexturePlayer = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iPlayer);
	TexturePlayer.iWidth_ = 96; 
	TexturePlayer.iHeight_ = 128;
	TexturePlayer.u_iData_ = chelik_dat;
	GLVM.LoadTextureData(TexturePlayer);
	GLVM::ECS::STransformComponent& TransformPlayer = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iPlayer);
    GLVM::ECS::CViewComponent& ViewPlayer = ComponentManager.GetComponent<GLVM::ECS::CViewComponent>(u_iPlayer);
    GLVM::ECS::CGravityComponent& GravityPlayer = ComponentManager.GetComponent<GLVM::ECS::CGravityComponent>(u_iPlayer);
    TransformPlayer.tVertex[0] = 0.0f;
	TransformPlayer.tVertex[1] = 1.0f;
	TransformPlayer.tVertex[2] = 4.5f;
    TransformPlayer.fScale = 0.5f;
    ViewPlayer.Front_Camera[0] = 0.0f;
    ViewPlayer.Front_Camera[1] = 0.0f;
    ViewPlayer.Front_Camera[2] = -1.0f;
    ViewPlayer.Up_Camera[0] = 0.0f;
    ViewPlayer.Up_Camera[1] = 1.0f;
    ViewPlayer.Up_Camera[2] = 0.0f;
    GravityPlayer.tGravity_[0] = 0.0f;
    GravityPlayer.tGravity_[1] = 0.0f;
    GravityPlayer.tGravity_[2] = 0.0f;
    
	Entity u_iWitch = 1;
	EntityManager.CreateEntity(u_iWitch);
	ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch);
	GLVM::ECS::CTextureComponent& TextureWitch = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch);
	TextureWitch.iWidth_ = 32;
	TextureWitch.iHeight_ = 32;
	TextureWitch.u_iData_ = witch_dat;
	GLVM.LoadTextureData(TextureWitch);
	GLVM::ECS::STransformComponent& TransformWitch = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch);
    TransformWitch.tVertex[0] = 3.5f;
    TransformWitch.tVertex[1] = 0.0f;
    TransformWitch.tVertex[2] = 0.0f;
    TransformWitch.fScale = 0.5f;

    Entity u_iWitch2;
	EntityManager.CreateEntity(u_iWitch2);
	ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch2);
	GLVM::ECS::CTextureComponent& TextureWitch2 = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch2);
	TextureWitch2.iWidth_ = 32;
	TextureWitch2.iHeight_ = 32;
	TextureWitch2.u_iData_ = witch_dat;
	GLVM.LoadTextureData(TextureWitch2);
	GLVM::ECS::STransformComponent& TransformWitch2 = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch2);
    TransformWitch2.tVertex[0] = 3.5f;
    TransformWitch2.tVertex[1] = -0.4f;
    TransformWitch2.tVertex[2] = 0.0f;
    TransformWitch2.fScale = 0.5f;

    // Entity u_iWitch3;
	// EntityManager.CreateEntity(u_iWitch3);
	// ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch3);
	// GLVM::ECS::CTextureComponent& TextureWitch3 = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch3);
	// TextureWitch3.iWidth_ = 32;
	// TextureWitch3.iHeight_ = 32;
	// TextureWitch3.u_iData_ = witch_dat;
	// GLVM.LoadTextureData(TextureWitch3);
	// GLVM::ECS::STransformComponent& TransformWitch3 = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch3);
    // TransformWitch3.tVertex[0] = 0.3f;
    // TransformWitch3.tVertex[1] = 0.5f;
    // TransformWitch3.tVertex[2] = 3.5f;
    // TransformWitch3.fScale = 0.5f;

    // Entity u_iWitch4;
	// EntityManager.CreateEntity(u_iWitch4);
	// ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch4);
	// GLVM::ECS::CTextureComponent& TextureWitch4 = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch4);
	// TextureWitch4.iWidth_ = 32;
	// TextureWitch4.iHeight_ = 32;
	// TextureWitch4.u_iData_ = witch_dat;
	// GLVM.LoadTextureData(TextureWitch4);
	// GLVM::ECS::STransformComponent& TransformWitch4 = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch4);
    // TransformWitch4.tVertex[0] = 0.3f;
    // TransformWitch4.tVertex[1] = 0.5f;
    // TransformWitch4.tVertex[2] = 3.0f;
    // TransformWitch4.fScale = 0.5f;

    // Entity u_iWitch5;
	// EntityManager.CreateEntity(u_iWitch5);
	// ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch5);
	// GLVM::ECS::CTextureComponent& TextureWitch5 = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch5);
	// TextureWitch5.iWidth_ = 32;
	// TextureWitch5.iHeight_ = 32;
	// TextureWitch5.u_iData_ = witch_dat;
	// GLVM.LoadTextureData(TextureWitch5);
	// GLVM::ECS::STransformComponent& TransformWitch5 = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch5);
    // TransformWitch5.tVertex[0] = 0.3f;
    // TransformWitch5.tVertex[1] = 0.5f;
    // TransformWitch5.tVertex[2] = 2.5f;
    // TransformWitch5.fScale = 0.5f;

    // Entity u_iWitch6;
	// EntityManager.CreateEntity(u_iWitch6);
	// ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch6);
	// GLVM::ECS::CTextureComponent& TextureWitch6 = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch6);
	// TextureWitch6.iWidth_ = 32;
	// TextureWitch6.iHeight_ = 32;
	// TextureWitch6.u_iData_ = witch_dat;
	// GLVM.LoadTextureData(TextureWitch6);
	// GLVM::ECS::STransformComponent& TransformWitch6 = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch6);
    // TransformWitch6.tVertex[0] = 0.3f;
    // TransformWitch6.tVertex[1] = 0.5f;
    // TransformWitch6.tVertex[2] = 2.0f;
    // TransformWitch6.fScale = 0.5f;
	
    ///< Game rendering loop
	GLVM.GameLoop(ComponentManager);
	GLVM.GameKill();

    return 0;
}
