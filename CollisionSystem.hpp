#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "VectorContainer.hpp"
#include "ConstVectorContainer.hpp"
#include "TransformComponent.hpp"
#include "ISystem.hpp"
#include "Event.hpp"
#include <iostream>
#include "MoveComponent.hpp"
#include "ColliderComponent.hpp"
#include "VectorContainer.hpp"

namespace GLVM::ECS
{
	class CCollisionSystem : public ISystem
	{
	public:
		double _dDelta_Time;
		void Repel(STransformComponent& _transform_Component, SMoveComponent& _move_Component, double& _fDelta_Time);
		bool BoxCollider(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2);
		void Update(ECS::CComponentManager& _Component_Manager) override;
	};
}
	
#endif
