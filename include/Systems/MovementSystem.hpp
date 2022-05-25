#ifndef MOVEMENT_SYSTEM
#define MOVEMENT_SYSTEM

#include "Event.hpp"
#include "Components/TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "Components/MoveComponent.hpp"
#include "ComponentManager.hpp"
#include "ISystem.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include "Stack.hpp"
#include "Globals.hpp"

namespace GLVM::ECS
{
	class CMovementSystem : public ISystem
	{
	public:
		float _dOffset;
		Core::EEvents _Anim_Event;
        Core::CStack& Input_Stack_;

        CMovementSystem(Core::CStack& _input_Stack);
        
		void Update();

        bool CompareDirection(Core::CStack& _input_Stack,
                              Core::EEvents _event0,
                              Core::EEvents _event1);
        void DiscardOpposite(Core::CStack& _input_Stack);
        Vector<float, 3> CalculateVectorRL(STransformComponent _transform_Component,
                                           ECS::CViewComponent& _view_Component,
                                           SMoveComponent& _move_Component,
                                           float _camera_Speed,
                                           Core::EEvents _current_Event);
        Vector<float, 3> CalculateVectorFB(STransformComponent _transform_Component,
                                           SMoveComponent& _move_Component,
                                           float _camera_Speed,
                                           ECS::CViewComponent& _view_Component,
                                           Core::CEvent& _event,
                                           Core::EEvents _current_Event);
        void CalculatePerdendicularVectors(float _camera_Speed,
                                           ECS::CViewComponent& _view_Component,
                                           Core::CEvent& _event,
                                           Vector<float, 3>& _temp_Vector);
        bool FixDiagonalMove(Core::CStack& _input_Stack,
                                         STransformComponent& _transform_Component,
                                         float _camera_Speed,
                                         ECS::CViewComponent& _view_Component,
                                         Core::CEvent& _event);
	};
}

#endif

