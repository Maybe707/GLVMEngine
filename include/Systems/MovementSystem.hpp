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
#include "ISoundEngine.hpp"
#include "Components/SpotLightComponent.hpp"

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
        void CalculateProjectile(ECS::CComponentManager* pComponent_Manager,
                                 unsigned int iEntity_refMove,
                                 CViewComponent& view_Component);
        
        bool CompareDirection(Core::CStack& _input_Stack,
                              Core::EEvents _event0,
                              Core::EEvents _event1);
        void DiscardOpposite(Core::CStack& _input_Stack);
        Vector<float, 3> CalculateVectorRL(ECS::CViewComponent& _view_Component);
        Vector<float, 3> CalculateVectorFB(SMoveComponent& _move_Component,
                                           ECS::CViewComponent& _view_Component,
                                           Core::CEvent& _event);
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

