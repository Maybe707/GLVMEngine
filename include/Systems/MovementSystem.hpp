// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef MOVEMENT_SYSTEM_HPP
#define MOVEMENT_SYSTEM_HPP

#include "Event.hpp"
#include "Components/TransformComponent.hpp"
#include "ISoundEngine.hpp"
#include "Vector.hpp"
#include "Components/MoveComponent.hpp"
#include "ComponentManager.hpp"
#include "ISystem.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include "EventsStack.hpp"
#include "Globals.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "ISoundEngine.hpp"
#include "Components/SpotLightComponent.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "ArchetypeECS/ArchetypeEntityManager.hpp"
#include "Archetypes/PlayerArchetype.hpp"

namespace GLVM::ecs
{
	class CMovementSystem : public ISystem
	{
	public:
		float deltaFrameTime;
		float gravity;
        core::CStack& inputStack;
		float prev_delta_x       = 0.0f;
		float prev_X             = 0.0f;
		float current_X          = 0.0f;
		vec3  prev_forward;
//		arch::componentMask mask = 
			
        
        CMovementSystem( core::CStack& inputStack );

		void Update();
        Vector<float, 3> CalculateVectorRL(components::beholder& beholder);
        Vector<float, 3> CalculateVectorFB(components::beholder& beholder,
                                           core::CEvent& event);
	};
}

#endif

