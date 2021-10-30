#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "VectorContainer.hpp"
#include "ISystem.hpp"

namespace GLVM::ECS
{
	class CCollisionSystem : public ISystem
	{
	public:
		// void Repel(CPlayer& _Player, double& _fDelta_Time, CEvent& _Event);
		// bool BoxCollider(CPlayer& _Player, IGameObject& _Game_Object);
		// void Detection(TCVectorContainer<IGameObject*>& _tWorld_Container, CPlayer& Player_Hero, double& Delta_Time, CEvent& _Event);
	};
}
	
#endif
