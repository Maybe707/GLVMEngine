#include "Systems/PhysicsSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "Globals.hpp"

namespace GLVM::ecs
{
    /*! This update searching for refering to colliders entities and check their
     *  transform components for collision, and if collision detected check if
     *  backtracking entity had gravity component for call Gravity function.
     */
         
    void CPhysicsSystem::Update() 
    {
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* componentManager = ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::collider,
																					  cm::move,
																					  cm::transform>();
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
        for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i)
        {
                unsigned int entityRefMove = linkedEntities[i];
				cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(entityRefMove);
				cm::move* move = componentManager->GetComponent<cm::move>(entityRefMove);
				cm::collider* collider = componentManager->GetComponent<cm::collider>(entityRefMove);
                if(collider->bGround_Collision_) {
					transformComponent->tPosition += move->gravity;
                    collider->bGround_Collision_ = false;
                }
                if(collider->bWall_Collision_) {
					transformComponent->tPosition += move->frameMovement;
                    collider->bWall_Collision_ = false;
                }
				move->frameMovement = 0.0f;
				componentManager->RemoveComponent<cm::move>(entityRefMove);
        }
    }
}
