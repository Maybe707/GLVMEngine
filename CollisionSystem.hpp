#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "VectorContainer.hpp"
#include "EventComponent.hpp"
#include "GravityComponent.hpp"
#include "TransformComponent.hpp"
#include "ISystem.hpp"
#include "Event.hpp"
#include <iostream>
#include "MoveComponent.hpp"
#include "ColliderComponent.hpp"
#include "VectorContainer.hpp"
#include "VertexMath.hpp"
#include "ViewComponent.hpp"

namespace GLVM::ECS
{
	class CCollisionSystem : public ISystem
	{
	public:
		double _dDelta_Time;
		void Repel(STransformComponent& _transform_Component, SMoveComponent& _move_Component,
                   double& _fDelta_Time, CViewComponent& _view_Component, SEventComponent& _event_Component);
		bool BoxCollider(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2);
		void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override;
	};
}
	
#endif
