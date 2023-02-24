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
		float                      deltaFrameTime;
        core::CStack&              inputStack;
        core::Sound::ISoundEngine* Sound_Engine_;

        float fYaw        = -90.0f;
        float fPitch      = 0.0f;
        float fLast_X     = 1920.0f / 2.0f;
        float fLast_Y     = 1080.0f / 2.0f;
        bool bFirst_Mouse = true;
        
        CMovementSystem( core::CStack& inputStack, core::Sound::ISoundEngine* soundEngine );

		void Update();
        bool TestDiagonalMovement(core::CStack& inputStack,
                              core::EEvents event1,
                              core::EEvents event2);
        Vector<float, 3> CalculateVectorRL(components::beholder& beholder);
        Vector<float, 3> CalculateVectorFB(components::beholder& beholder,
                                           core::CEvent& event);
        vec3 CalculatePerdendicularVectors(float speed,
                                           components::beholder& beholder,
                                           core::CEvent& event);
	};
}

#endif

