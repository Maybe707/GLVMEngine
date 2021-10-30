#include "Engine.hpp"
#include "SpritesData.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"

using Entity = unsigned int;

int main()
{
	GLVM::Core::CEngine GLVM;
	GLVM::ECS::CEntityManager EntityManager;
	GLVM::ECS::CComponentManager ComponentManager;
	Entity u_iPlayer = 0;
	EntityManager.CreateEntity(u_iPlayer);

	GLVM::ECS::CTextureComponent TexturePlayer;
	TexturePlayer.iWidth_ = 96;
	TexturePlayer.iHeight_ = 128;
	TexturePlayer.u_iData_ = chelik_dat;
//	GLVM::ECS::CColliderComponent ColliderPlayer;
	GLVM::ECS::STransformComponent TransformPlayer;

	ComponentManager.BindComponentToEntity<GLVM::ECS::CTextureComponent>
		(u_iPlayer, TexturePlayer);
	ComponentManager.BindComponentToEntity<GLVM::ECS::STransformComponent>
		(u_iPlayer, TransformPlayer);
	
	// GLVM::Core::CPlayer* pPlayer = new GLVM::Core::CPlayer(Texture);
	// GLVM::Core::CStaticObject* pWall = new GLVM::Core::CStaticObject(Texture2);
	// GLVM::Core::CStaticObject* pWall2 = new GLVM::Core::CStaticObject(Texture2);
	// GLVM::Core::CStaticObject* pWall3 = new GLVM::Core::CStaticObject(Texture2);

	// GLVM::ECS::CMatrixComponent Matrix2(128.0f);
 	// Matrix2.Offset({0.0f, 0.0f});
	// GLVM::ECS::CMatrixComponent Matrix3(64.0f);
 	// Matrix3.Offset({64.0f, -150.0f});
	// GLVM::ECS::CMatrixComponent Matrix4(64.0f);
	// Matrix4.Offset({164.0f, 100.0f});
	// GLVM::ECS::CMatrixComponent Matrix5(64.0f);
 	// Matrix5.Offset({-105.0f, 0.0f});
	
	// pPlayer->SetMatrix(tMatrix2);
	// pWall->SetMatrix(tMatrix3);
	// pWall2->SetMatrix(tMatrix4);
	// pWall3->SetMatrix(tMatrix5);
//	GLVM.tWorldContainer.Push(pPlayer);
 	// GLVM.GetWorldContainer().Push(pWall);
	// GLVM.GetWorldContainer().Push(pWall2);
	// GLVM.GetWorldContainer().Push(pWall3);

    ///< Game rendering loop
	GLVM.GameLoop(ComponentManager);
	GLVM.GameKill();
	
    return 0;
}
