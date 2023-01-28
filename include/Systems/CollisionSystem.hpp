#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "VectorContainer.hpp"
#include "Components/EventComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "ISystem.hpp"
#include "Event.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "VectorContainer.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include <mutex>

namespace GLVM::ECS
{
	class CCollisionSystem : public ISystem
	{   
	public:
        
		float fDelta_Time_;
        Core::CStack& Input_Stack_;

        CCollisionSystem(Core::CStack& _input_Stack) : Input_Stack_(_input_Stack) {}
		void Repel(transform& _transform_Component,
                   move& _move_Component,
                                 float& _fDelta_Time,
                                 beholder& _view_Component,
                                 Core::CEvent& _event);
        bool Gravity(transform& _transform_Component, event& _event_Component);
		bool BoxCollider(transform& _transform_Component1, transform& _transform_Component2);
		void Update() override;
        bool UpperActorCheck(transform& _transform_Component1,
                             transform& _transform_Component2);
    };
}
	
#endif
