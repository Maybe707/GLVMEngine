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
    ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent, GLVM::ECS::CTextureComponent, GLVM::ECS::SMoveComponent, GLVM::ECS::CColliderComponent, GLVM::ECS::SAnimationMoveComponent, GLVM::ECS::CViewComponent, GLVM::ECS::STransformComponent>(u_iPlayer);
    GLVM::ECS::CTextureComponent& TexturePlayer = ComponentManager.GetComponent<GLVM::ECS::CTextureComponent>(u_iPlayer);
	TexturePlayer.iWidth_ = 96; 
	TexturePlayer.iHeight_ = 128;
	TexturePlayer.u_iData_ = chelik_dat;
	GLVM.LoadTextureData(TexturePlayer);
	GLVM::ECS::STransformComponent& TransformPlayer = ComponentManager.GetComponent<GLVM::ECS::STransformComponent>(u_iPlayer);
	TransformPlayer.fPos_X = 0;
	TransformPlayer.fPos_Y = 0;
	TransformPlayer.fPos_Z = 150;
    TransformPlayer.fRotate = 30.0f;
    TransformPlayer.fScale = 120.0f;
    
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
	TransformWitch.fPos_X = 0;
	TransformWitch.fPos_Y = 0;
	TransformWitch.fPos_Z = 100;
    TransformWitch.fScale = 64.0f;
	// EntityManager.RemoveEntity(u_iWitch, ComponentManager);
    // EntityManager.tActive_Entity_Registry_.Print();    

    ///< Game rendering loop
	GLVM.GameLoop(ComponentManager);
	GLVM.GameKill();

    return 0;
}
