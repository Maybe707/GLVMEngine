#include "Engine.hpp"
#include "GameObject.hpp"
#include "Player.hpp"
#include "StaticObject.hpp"

int main()
{
	GLVM::Core::Engine GLVM;
    GLVM::Core::CTexture Texture("../textures/hero.png");
	GLVM::Core::CTexture Texture2("../textures/enemy.png");
	GLVM::Core::CPlayer* pPlayer = new GLVM::Core::CPlayer(Texture);
	GLVM::Core::CStaticObject* pWall = new GLVM::Core::CStaticObject(Texture2);
	GLVM::Math::TCMatrix<4> tMatrix2(0.1f);
 	tMatrix2.Offset(-0.2f);

	GLVM.tWorldContainer.Push(pPlayer);
	GLVM.tWorldContainer.Push(pWall);
	
    ///< Game rendering loop
	GLVM.GameLoop(*pPlayer);
	GLVM.GameKill();
    
    return 0;
}
