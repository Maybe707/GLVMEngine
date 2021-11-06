#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "VectorContainer.hpp"
#include "ConstVectorContainer.hpp"
#include "TransformComponent.hpp"
#include "ISystem.hpp"
#include "Event.hpp"
#include <iostream>
#include "MoveComponent.hpp"

namespace GLVM::ECS
{
	class CCollisionSystem : public ISystem
	{
	public:
		void Repel(STransformComponent& _transform_Component, SMoveComponent& _move_Component, double& _fDelta_Time);
		bool BoxCollider(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2);
		void Detection(Core::TCConstVectorContainer<STransformComponent>* _pTransform_Components_Container, Core::TCVectorContainer<unsigned int>* pOrdered_Colliders_Container, Core::TCConstVectorContainer<SMoveComponent>* _pMove_Components_Container, Core::TCVectorContainer<unsigned int>* pOrdered_Move_Container, double& _dDelta_Time);
	};
}
	
#endif
