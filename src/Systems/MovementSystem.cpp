// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Systems/MovementSystem.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ControllerComponent.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Engine.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "ISoundEngine.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include <cstdint>
#include <cstdio>

namespace GLVM::ecs
{
    CMovementSystem::CMovementSystem(core::CStack& inputStack) :
        inputStack(inputStack) {}
        
    void CMovementSystem::Update()
    {
		namespace cm   = GLVM::ecs::components;
		namespace arch = GLVM::ecs::arch;

		arch::PlayerArchetype* playerArch = {};
		uint32_t entityCount = 0;
		if( arch::world.archetypes.GetSize() > 1 ) {
			playerArch = static_cast<arch::PlayerArchetype*>(arch::world.archetypes[1]);
			entityCount = playerArch->entityCount;
		}
        const float cameraSpeed = 1.0f * deltaFrameTime;            

        for(unsigned int i = 0; i < entityCount; ++i) {
			cm::beholder* beholderComponent     = &playerArch->beholders[i];
            for(int n = 0; n < 6; ++n) {
				vec3 right;
				vec3 forward;
                switch(inputStack[n])
                {
                case core::EEvents::eMOVE_LEFT:
					right = CalculateVectorRL(*beholderComponent);
					playerArch->moves[i].frameMovement -= right * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_RIGHT:
					right = CalculateVectorRL(*beholderComponent);
					playerArch->moves[i].frameMovement += right * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_BACKWARD:
                    forward = CalculateVectorFB(*beholderComponent, g_eEvent);
					playerArch->moves[i].frameMovement -= forward * cameraSpeed;
                    break;
                case core::EEvents::eMOVE_FORWARD:
					forward = CalculateVectorFB(*beholderComponent, g_eEvent);
					playerArch->moves[i].frameMovement += forward * cameraSpeed;
                    break;
                case core::EEvents::eJUMP:
				{
					if ( playerArch->colliderFlags[i].flags & 1 ) {
						playerArch->rigidBodies[i].jumpAccumulator = 1.5f;
					}
				}
                    break;
                default:
                    break;
                }
            }
        }

		for(unsigned int n = 0; n < entityCount; ++n) {
			cm::transform* rTransform_Component = &playerArch->transforms[n];
			cm::rigidBody* rigidBodyComponennt  = &playerArch->rigidBodies[n];
			cm::move* moveComponent             = &playerArch->moves[n];
			rTransform_Component->gravityAccumulator += deltaFrameTime;
			float gravity = 9.8f * rTransform_Component->gravityAccumulator
				* rigidBodyComponennt->fMass_ * 0.0005;
			if ( gravity > 0.2f )
				gravity = 0.2;

			moveComponent->gravity[1] -= gravity;
        }

		arch::EnemyArchetype* enemyArch = {};
		entityCount = 0;
		if( arch::world.archetypes.GetSize() > 2 ) {
			enemyArch = static_cast<arch::EnemyArchetype*>(arch::world.archetypes[2]);
			entityCount = enemyArch->entityCount;
		}
		for(unsigned int n = 0; n < entityCount; ++n) {
			cm::transform* rTransform_Component = &enemyArch->transforms[n];
			cm::rigidBody* rigidBodyComponennt  = &enemyArch->rigidBodies[n];
			cm::move* moveComponent             = &enemyArch->moves[n];
			rTransform_Component->gravityAccumulator += deltaFrameTime;
			float gravity = 9.8f * rTransform_Component->gravityAccumulator
				* rigidBodyComponennt->fMass_ * 0.0005;
			if ( gravity > 0.2f )
				gravity = 0.2;

			moveComponent->gravity[1] -= gravity;
        }
    }

    Vector<float, 3> CMovementSystem::CalculateVectorRL(components::beholder& beholder) {
        Vector<float, 3> normalizedVector = Normalize(Cross(beholder.forward, vec3{ 0.0f, -1.0f, 0.0 }));
        return normalizedVector;
    }

    Vector<float, 3> CMovementSystem::CalculateVectorFB(components::beholder& beholder,
                                                        [[maybe_unused]] core::CEvent& event) {
        Vector<float, 3> forward(0.0f);
        // forward[0] = std::cos(Radians(event.mousePointerPosition.yaw * 2));
        // forward[2] = std::sin(Radians(event.mousePointerPosition.yaw * 2));

		// float sinYaw = std::sin(Radians(event.mousePointerPosition.yaw / 2));
		// float cosYaw = std::cos(Radians(event.mousePointerPosition.yaw / 2));
		
		// Quaternion yawQuat;
		// yawQuat.w = cosYaw;
		// yawQuat.x = 0.0f;
		// yawQuat.y = sinYaw;
		// yawQuat.z = 0.0f;

		// Quaternion result;
		// result = multiplyQuaternion(multiplyQuaternion(yawQuat, Quaternion{ .w = 0.0f, .x = 0.0f,
		// 			.y = 0.0f, .z = 1.0f }), inverseQuaternion(yawQuat));

		// forward[0] = result.x;
		// forward[1] = result.y;
		// forward[2] = result.z;

		current_X = (float)g_eEvent.mousePointerPosition.offset_X;
		float delta_x = current_X - prev_X;
		// if ( delta_x < 0.0001 )
		// 	delta_x = prev_delta_x;

		const vec3 rotateAxis = { 0.0, -1.0, 0.0 };
		float rotationAngle = delta_x;
		constexpr float angleScale = 0.1f;
		rotationAngle = Radians(rotationAngle * angleScale);
		constexpr float quatAngleCorrection = 0.5f;                                                                                     /// Quaternions need devision by 2
		const float sinRotationAngle = sinf(rotationAngle * quatAngleCorrection);
		Quaternion rotationQuat = Quaternion(cosf(rotationAngle * quatAngleCorrection), sinRotationAngle * rotateAxis[0],
											 sinRotationAngle * rotateAxis[1], sinRotationAngle * rotateAxis[2]);
		// Quaternion appliedRotationQuat = multiplyQuaternion(multiplyQuaternion(rotationQuat, Quaternion(0.0f, beholder.forward[0],
		// 																								beholder.forward[1], beholder.forward[2])),
		// 													conjugate(rotationQuat));
		const Quaternion appliedRotationQuat = (rotationQuat * Quaternion(0.0f, beholder.forward[0], beholder.forward[1],
																		  beholder.forward[2])) * conjugate(rotationQuat);
		
		
		forward[0] = appliedRotationQuat.x;
		forward[1] = 0.0f;
		forward[2] = appliedRotationQuat.z;

		prev_X = (float)g_eEvent.mousePointerPosition.offset_X;
		// if ( delta_x > 0.0f )
		// 	prev_delta_x = delta_x;
		
        forward = Normalize(forward);
        return forward;
    }
}


