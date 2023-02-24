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
        // core::vector<unsigned int>* entityContainerRefMove =
		// 	componentManager->GetEntityContainer<cm::move>();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::collider,
																					  cm::move,
																					  cm::transform>();
		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();

//        unsigned int uiVector_Collider_Size = entityContainerRefMove->GetSize();

        for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i)
        {
                unsigned int entityRefMove = linkedEntities[i];
				cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(entityRefMove);
				cm::move* move = componentManager->GetComponent<cm::move>(entityRefMove);
				cm::collider* collider = componentManager->GetComponent<cm::collider>(entityRefMove);
                if(collider->bGround_Collision_) {
					move->frameMovement[1] = 0.0f;
                    collider->bGround_Collision_ = false;
                }
                if(collider->bWall_Collision_) {
					move->frameMovement[0] = 0.0f;
					move->frameMovement[2] = 0.0f;					
                    collider->bWall_Collision_ = false;
                }
					// std::cout << "vec: " << move->frameMovement[0] << " " << move->frameMovement[1] <<
					// 	" " << move->frameMovement[2] << " For entity: " << entityRefMove << std::endl;
					// unsigned int Size = entityContainerRefMove->GetSize();
					// std::cout << "size 1: " << Size << std::endl;
					transformComponent->tPosition += move->frameMovement;
					move->frameMovement = 0.0f;
//					std::cout << "i: " << i << std::endl;
					componentManager->RemoveComponent<cm::move>(entityRefMove);
				// Size = entityContainerRefMove->GetSize();
				// std::cout << "size 2: " << Size << std::endl;
        }
    }
}
