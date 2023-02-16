#ifndef MOVEMENT_SYSTEM
#define MOVEMENT_SYSTEM

#include "Event.hpp"
#include "Components/TransformComponent.hpp"
#include "ISoundEngine.hpp"
#include "Vector.hpp"
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

namespace GLVM::ecs
{
	class CMovementSystem : public ISystem
	{
	public:
        float                      fProjectile_Accumulator_ = 2.0f; 
		float                      _dOffset;
        core::CStack&              Input_Stack_;
        core::Sound::ISoundEngine* Sound_Engine_;

        float fYaw        = -90.0f;
        float fPitch      = 0.0f;
        float fLast_X     = 1920.0f / 2.0f;
        float fLast_Y     = 1080.0f / 2.0f;
        bool bFirst_Mouse = true;
        
        CMovementSystem( core::CStack& _input_Stack, core::Sound::ISoundEngine* _sound_Engine );

		void Update();
        void CalculateProjectile( ecs::CComponentManager* pComponent_Manager,
                                  unsigned int iEntity_refMove,
                                  components::beholder& view_Component );
        
        bool CompareDirection(core::CStack& _input_Stack,
                              core::EEvents _event0,
                              core::EEvents _event1);
        Vector<float, 3> CalculateVectorRL(components::beholder& _view_Component);
        Vector<float, 3> CalculateVectorFB(components::beholder& _view_Component,
                                           core::CEvent& _event);
        vec3 CalculatePerdendicularVectors(float speed,
                                           components::beholder& beholder,
                                           core::CEvent& _event);
        bool FixDiagonalMove(core::CStack& _input_Stack,
							 components::transform& _transform_Component,
							 float _camera_Speed,
							 components::beholder& _view_Component,
							 components::move& moveComponent,
							 core::CEvent& _event);
	};
}

#endif

