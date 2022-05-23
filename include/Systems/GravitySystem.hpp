#ifndef GRAVITY_SYSTEM
#define GRAVITY_SYSTEM

#include "Components/ColliderComponent.hpp"
#include "../ComponentManager.hpp"
#include "../Event.hpp"
#include "Components/EventComponent.hpp"
#include "Components/GravityComponent.hpp"
#include "ISystem.hpp"
#include "Components/TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "Stack.hpp"
#include "Components/ViewComponent.hpp"

namespace GLVM::ECS
{

    class CGravitySystem : public ECS::ISystem
    {
    public:
        float fAcceleration_of_Gravity_;
        float fDelta_Time_;
        Core::CStack& Input_Stack_;

        CGravitySystem(Core::CStack& _input_Stack) : Input_Stack_(_input_Stack) {}
        
        ///< Set Y-axis of transform component of backtracking entity to upper Y-axis of ground entity.
        
        void Gravity(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2);

        /*! This update searching for refering to colliders entities and check their
         *  transform components for collision, and if collision detected check if
         *  backtracking entity had gravity component for call Gravity function.
         */
         
        void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override;
        void Repel(STransformComponent& _transform_Component,
                   SMoveComponent& _move_Component,
                   float& _fDelta_Time,
                   CViewComponent& _view_Component,
                   Core::CEvent& _event);
    };
}

#endif
