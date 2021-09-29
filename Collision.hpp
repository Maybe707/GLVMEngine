#ifndef COLLISION_H
#define COLLISION_H

#include "Player.hpp"
#include "VectorContainer.hpp"
//#include "TileMap.hpp"

namespace GLVM::Core
{
    
class CCollision
{
public:
	void Repel(CPlayer& _Player, double& _fDelta_Time, CEvent& _Event);
	bool BoxCollider(CPlayer& _Player, IGameObject& _Game_Object);
	void Detection(TCVectorContainer<IGameObject*>& _tWorld_Container, CPlayer& Player_Hero, double& Delta_Time, CEvent& _Event);
};

}
	
#endif
