#include "Engine.hpp"
#include "SpritesData.hpp"

int main()
{
	GLVM::Core::CEngine GLVM;
	GLVM::ECS::CEntityManager EntityManager;
	GLVM::ECS::CComponentManager ComponentManager;
	Entity u_iPlayer = 0;
	EntityManager.CreateEntity(u_iPlayer);
    ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent>(u_iPlayer);
	GLVM::ECS::CTextureComponent& TexturePlayer = ComponentManager.CreateComponent<GLVM::ECS::CTextureComponent>(u_iPlayer);
	ComponentManager.CreateComponent<GLVM::ECS::SMoveComponent>(u_iPlayer);
	ComponentManager.CreateComponent<GLVM::ECS::CColliderComponent>(u_iPlayer);
	ComponentManager.CreateComponent<GLVM::ECS::SAnimationMoveComponent>(u_iPlayer);
	TexturePlayer.iWidth_ = 96;
	TexturePlayer.iHeight_ = 128;
	TexturePlayer.u_iData_ = chelik_dat;
	GLVM.LoadTextureData(TexturePlayer);
	GLVM::ECS::STransformComponent& TransformPlayer = ComponentManager.CreateComponent<GLVM::ECS::STransformComponent>(u_iPlayer);
	TransformPlayer.fPos_X = 5;
	TransformPlayer.fPos_Y = 3;
	TransformPlayer.fPos_Z = 3;
	
	Entity u_iWitch = 0;
	EntityManager.CreateEntity(u_iWitch);
	ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent>(u_iWitch);
	GLVM::ECS::CTextureComponent& TextureWitch = ComponentManager.CreateComponent<GLVM::ECS::CTextureComponent>(u_iWitch);
	ComponentManager.CreateComponent<GLVM::ECS::SMoveComponent>(u_iWitch);
	ComponentManager.CreateComponent<GLVM::ECS::CColliderComponent>(u_iWitch);
	TextureWitch.iWidth_ = 32;
	TextureWitch.iHeight_ = 32;
	TextureWitch.u_iData_ = witch_dat;
	GLVM.LoadTextureData(TextureWitch);
	GLVM::ECS::STransformComponent& TransformWitch = ComponentManager.CreateComponent<GLVM::ECS::STransformComponent>(u_iWitch);
	TransformWitch.fPos_X = 90;
	TransformWitch.fPos_Y = 90;
	TransformWitch.fPos_Z = 90;
//	EntityManager.RemoveEntity(u_iWitch, ComponentManager);

	Entity u_iWitch2 = 0;
	EntityManager.CreateEntity(u_iWitch2);
   	ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent>(u_iWitch2);
	GLVM::ECS::CTextureComponent& TextureWitch2 = ComponentManager.CreateComponent<GLVM::ECS::CTextureComponent>(u_iWitch2);
	ComponentManager.CreateComponent<GLVM::ECS::CColliderComponent>(u_iWitch2);
	TextureWitch2.iWidth_ = 32;
	TextureWitch2.iHeight_ = 32;
	TextureWitch2.u_iData_ = witch_dat;
	GLVM.LoadTextureData(TextureWitch2);
	GLVM::ECS::STransformComponent& TransformWitch2 = ComponentManager.CreateComponent<GLVM::ECS::STransformComponent>(u_iWitch2);
	TransformWitch2.fPos_X = 190;
	TransformWitch2.fPos_Y = 190;
	TransformWitch2.fPos_Z = 190;

	Entity u_iWitch3 = 0;
	EntityManager.CreateEntity(u_iWitch3);
	ComponentManager.CreateComponent<GLVM::ECS::SVertexComponent>(u_iWitch3);
	GLVM::ECS::CTextureComponent& TextureWitch3 = ComponentManager.CreateComponent<GLVM::ECS::CTextureComponent>(u_iWitch3);
	ComponentManager.CreateComponent<GLVM::ECS::CColliderComponent>(u_iWitch3);
	TextureWitch3.iWidth_ = 32;
	TextureWitch3.iHeight_ = 32;
	TextureWitch3.u_iData_ = witch_dat;
	GLVM.LoadTextureData(TextureWitch3);
	GLVM::ECS::STransformComponent& TransformWitch3 = ComponentManager.CreateComponent<GLVM::ECS::STransformComponent>(u_iWitch3);
	TransformWitch3.fPos_X = 130;
	TransformWitch3.fPos_Y = -140;
	TransformWitch3.fPos_Z = 150;

    ///< Game rendering loop
	GLVM.GameLoop(ComponentManager);
	GLVM.GameKill();

    return 0;
}
