#include "Engine.hpp"
#include "SpritesData.hpp"

int main()
{
	GLVM::Core::CEngine GLVM;
	GLVM::Core::CTexture Texture(96, 128, chelik_dat);
	GLVM::Core::CTexture Texture2(32, 32, witch_dat);
	GLVM::Core::CPlayer* pPlayer = new GLVM::Core::CPlayer(Texture);
	GLVM::Core::CStaticObject* pWall = new GLVM::Core::CStaticObject(Texture2);
	GLVM::Core::CStaticObject* pWall2 = new GLVM::Core::CStaticObject(Texture2);
	GLVM::Core::CStaticObject* pWall3 = new GLVM::Core::CStaticObject(Texture2);
	GLVM::Math::TCMatrix<4> tMatrix2(128.0f);
 	tMatrix2.Offset({0.0f, 0.0f});
	GLVM::Math::TCMatrix<4> tMatrix3(64.0f);
 	tMatrix3.Offset({64.0f, -150.0f});
	GLVM::Math::TCMatrix<4> tMatrix4(64.0f);
	tMatrix4.Offset({164.0f, 100.0f});
	GLVM::Math::TCMatrix<4> tMatrix5(64.0f);
 	tMatrix5.Offset({-105.0f, 0.0f});
	
	pPlayer->SetMatrix(tMatrix2);
	pWall->SetMatrix(tMatrix3);
	pWall2->SetMatrix(tMatrix4);
	pWall3->SetMatrix(tMatrix5);
//	GLVM.tWorldContainer.Push(pPlayer);
 	GLVM.GetWorldContainer().Push(pWall);
	GLVM.GetWorldContainer().Push(pWall2);
	GLVM.GetWorldContainer().Push(pWall3);

    ///< Game rendering loop
	GLVM.GameLoop(*pPlayer);
	GLVM.GameKill();
	
    return 0;
}
