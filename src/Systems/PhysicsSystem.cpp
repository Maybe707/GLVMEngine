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
		
        CComponentManager* pComponent_Manager = CComponentManager::GetInstance();
        core::vector<unsigned int>* entityContainerRefMove =
			pComponent_Manager->GetEntityContainer<cm::move>();
//        unsigned int uiVector_Collider_Size = entityContainerRefMove->GetSize();

        for(unsigned int i = 0; i < entityContainerRefMove->GetSize(); ++i)
        {
                unsigned int entityRefMove = (*entityContainerRefMove)[i];
				cm::transform& transformComponent = pComponent_Manager->GetComponent<cm::transform>(entityRefMove);
				cm::move& move = pComponent_Manager->GetComponent<cm::move>(entityRefMove);
                if(pComponent_Manager->GetComponent<cm::collider>(entityRefMove).bGround_Collision_) {
					move.frameMovement[1] = 0.0f;
                    pComponent_Manager->GetComponent<cm::collider>(entityRefMove).bGround_Collision_ = false;
                }
                if(pComponent_Manager->GetComponent<cm::collider>(entityRefMove).bWall_Collision_) {
					move.frameMovement[0] = 0.0f;
					move.frameMovement[2] = 0.0f;					
                    pComponent_Manager->GetComponent<cm::collider>(entityRefMove).bWall_Collision_ = false;
                }
//				std::cout << entityContainerRefMove->GetSize() << std::endl;
				std::cout << "vec: " << move.frameMovement[0] << " " << move.frameMovement[1] <<
					" " << move.frameMovement[2] << " For entity: " << entityRefMove << std::endl;
				// unsigned int Size = entityContainerRefMove->GetSize();
				// std::cout << "size 1: " << Size << std::endl;
				transformComponent.tPosition += move.frameMovement;
				move.frameMovement = 0.0f;
				pComponent_Manager->RemoveComponent<cm::move>(entityRefMove);
				// Size = entityContainerRefMove->GetSize();
				// std::cout << "size 2: " << Size << std::endl;
        }
    }
}
