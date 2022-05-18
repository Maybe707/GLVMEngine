#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "VectorContainer.hpp"
#include "Components/EventComponent.hpp"
#include "Components/GravityComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "ISystem.hpp"
#include "Event.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "VectorContainer.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"

namespace GLVM::ECS
{
	class CCollisionSystem : public ISystem
	{
	public:
		float fDelta_Time_;
		void Repel(STransformComponent& _transform_Component, SMoveComponent& _move_Component,
                   float& _fDelta_Time, CViewComponent& _view_Component, SEventComponent& _event_Component);
        bool Gravity(STransformComponent& _transform_Component, SEventComponent& _event_Component);
		bool BoxCollider(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2, SEventComponent& _event_Component, float& _fDelta_Time);
		void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override;
	};
}
	
#endif
