#include "AnimationMoveComponent.hpp"
#include "ColliderComponent.hpp"
#include "Engine.hpp"
#include "MoveComponent.hpp"
#include "SpritesData.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include "VertexComponent.hpp"
#include "ViewComponent.hpp"

int main()
{
	GLVM::Core::CEngine GLVM;
	GLVM::ECS::CEntityManager EntityManager;
	GLVM::ECS::CComponentManager ComponentManager;
    
	Entity u_iPlayer;
	EntityManager.CreateEntity(u_iPlayer);
    ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::SMoveComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::SAnimationMoveComponent, GLVM::ECS::CViewComponent, GLVM::ECS::STransformComponent>(u_iPlayer);
    // GLVM::ECS::CTextureComponent& TexturePlayer = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iPlayer);
	// TexturePlayer.iWidth_ = 96; 
	// TexturePlayer.iHeight_ = 128;
//	TexturePlayer.u_iData_ = chelik_dat;
//	GLVM.LoadTextureData(TexturePlayer);
	GLVM::ECS::STransformComponent& TransformPlayer = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iPlayer);
    GLVM::ECS::CViewComponent& ViewPlayer = ComponentManager.GetComponent<GLVM::ECS::CViewComponent>(u_iPlayer);
	// TransformPlayer.fPos_X = 150.0f;
	// TransformPlayer.fPos_Y = 100.0f;
	// TransformPlayer.fPos_Z = 30.0f;
    // TransformPlayer.fRotate = 30.0f;
    // TransformPlayer.fScale = 64.0f;
    
    TransformPlayer.tVertex[0] = 0.0f;
	TransformPlayer.tVertex[1] = 0.0f;
	TransformPlayer.tVertex[2] = 3.0f;
    // TransformPlayer.fRotate = 0.0f;
    TransformPlayer.fScale = 0.1f;

    ViewPlayer.Front_Camera[0] = 0.0f;
    ViewPlayer.Front_Camera[1] = 0.0f;
    ViewPlayer.Front_Camera[2] = -1.0f;
    ViewPlayer.Up_Camera[0] = 0.0f;
    ViewPlayer.Up_Camera[1] = 1.0f;
    ViewPlayer.Up_Camera[2] = 0.0f;
    
	Entity u_iWitch;
	EntityManager.CreateEntity(u_iWitch);
	ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch);
	GLVM::ECS::CTextureComponent& TextureWitch = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch);
	ComponentManager.CreateComponent<GLVM::ECS::CColliderComponent>(u_iWitch);
	TextureWitch.iWidth_ = 32;
	TextureWitch.iHeight_ = 32;
	TextureWitch.u_iData_ = witch_dat;
	GLVM.LoadTextureData(TextureWitch);
	GLVM::ECS::STransformComponent& TransformWitch = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch);
	// TransformWitch.fPos_X = 100.0f;
	// TransformWitch.fPos_Y = 150.0f;
	// TransformWitch.fPos_Z = 100.0f;
    // TransformWitch.fScale = 64.0f;
    
    TransformWitch.tVertex[0] = 0.0f;
    TransformWitch.tVertex[1] = 0.0f;
    TransformWitch.tVertex[2] = 3.5f;
    TransformWitch.fScale = 1.0f;
	// EntityManager.RemoveEntity(u_iWitch, ComponentManager);
    // EntityManager.tActive_Entity_Registry_.Print();

    Entity u_iWitch2;
	EntityManager.CreateEntity(u_iWitch2);
	ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::STransformComponent>(u_iWitch2);
	GLVM::ECS::CTextureComponent& TextureWitch2 = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iWitch2);
	ComponentManager.CreateComponent<GLVM::ECS::CColliderComponent>(u_iWitch2);
	TextureWitch2.iWidth_ = 32;
	TextureWitch2.iHeight_ = 32;
	TextureWitch2.u_iData_ = witch_dat;
	GLVM.LoadTextureData(TextureWitch2);
	GLVM::ECS::STransformComponent& TransformWitch2 = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iWitch2);
	// TransformWitch.fPos_X = 100.0f;
	// TransformWitch.fPos_Y = 150.0f;
	// TransformWitch.fPos_Z = 100.0f;
    // TransformWitch.fScale = 64.0f;
    
    TransformWitch2.tVertex[0] = 0.3f;
    TransformWitch2.tVertex[1] = 0.2f;
    TransformWitch2.tVertex[2] = 4.0f;
    TransformWitch2.fScale = 1.0f;
	// EntityManager.RemoveEntity(u_iWitch, ComponentManager);
    // EntityManager.tActive_Entity_Registry_.Print();    
	
    ///< Game rendering loop
	GLVM.GameLoop(ComponentManager);
	GLVM.GameKill();

    return 0;
}
