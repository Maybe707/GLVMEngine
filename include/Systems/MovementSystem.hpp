#ifndef MOVEMENT_SYSTEM
#define MOVEMENT_SYSTEM

#include "Event.hpp"
#include "Components/TransformComponent.hpp"
#include "ISoundEngine.hpp"
#include "VectorContainer.hpp"
#include "Components/MoveComponent.hpp"
#include "ComponentManager.hpp"
#include "ISystem.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include "Stack.hpp"
#include "Globals.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "chelik.hpp"
#include "ISoundEngine.hpp"

namespace GLVM::ECS
{
	class CMovementSystem : public ISystem
	{
	public:
        float                      fProjectile_Accumulator_ = 2.0f; 
		float                      _dOffset;
		Core::EEvents              _Anim_Event;
        Core::CStack&              Input_Stack_;
        Core::Sound::ISoundEngine* Sound_Engine_;

        float fYaw        = -90.0f;
        float fPitch      = 0.0f;
        float fLast_X     = 1920.0f / 2.0f;
        float fLast_Y     = 1080.0f / 2.0f;
        bool bFirst_Mouse = true;
        
        CMovementSystem(Core::CStack& _input_Stack, Core::Sound::ISoundEngine* _sound_Engine);
        
		void Update();

        Vector<float, 3> GetDirectionVector(ECS::STransformComponent& _Player, ECS::CViewComponent& _view_Component);
        
        void CalculateProjectile(ECS::CComponentManager* pComponent_Manager,
                                 unsigned int iEntity_refMove,
                                 float cameraSpeed,
                                 CViewComponent& view_Component,
                                 Core::CStack& Input_Stack_,
                                 int n);
        
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
        Vector<float, 3> CalculateForwardVectorProjectile(STransformComponent _transform_Component,
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

