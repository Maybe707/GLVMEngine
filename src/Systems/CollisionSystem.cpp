#include "Systems/CollisionSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "EntityManager.hpp"
#include "Event.hpp"
#include "Components/EventComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Stack.hpp"
#include "Vector.hpp"

namespace GLVM::ecs
{
	bool CCollisionSystem::BoxCollider(vec3 backtrackingPosition, vec3 comparedPosition,
		                               float backtrackingScale, float comparedScale)
	{
        if(backtrackingPosition[0] + backtrackingScale / 2 > comparedPosition[0] - comparedScale / 2 &&
           backtrackingPosition[0] - backtrackingScale / 2 < comparedPosition[0] + comparedScale / 2 &&
           backtrackingPosition[1] + backtrackingScale / 2 > comparedPosition[1] - comparedScale / 2 &&
           backtrackingPosition[1] - backtrackingScale / 2 < comparedPosition[1] + comparedScale / 2 &&
           backtrackingPosition[2] + backtrackingScale / 2 > comparedPosition[2] - comparedScale / 2 &&
           backtrackingPosition[2] - backtrackingScale / 2 < comparedPosition[2] + comparedScale / 2) {
				return true;
		}
        
		return false;
	}

    bool CCollisionSystem::UpperActorCheck(vec3 backtrackingPosition, vec3 comparedPosition,
										   float backtrackingScale, float comparedScale) {
        if((backtrackingPosition[1] - backtrackingScale / 2) >
		   (comparedPosition[1] + (comparedScale / 2  - (comparedScale / 10)))) {
            return true;
        }

        return false;
    }

	
	void CCollisionSystem::Update()
	{
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* componentManager = ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities = componentManager->collectLinkedEntities<cm::collider,
																					  cm::transform>();

		unsigned int linkedEntitiesVectorSize = linkedEntities.GetSize();
		for(unsigned int i = 0; i < linkedEntitiesVectorSize; ++i) {
			for(unsigned int j = i + 1; j < linkedEntitiesVectorSize; ++j) {
                unsigned int backtrackingEntityRefCollider = linkedEntities[i];  
                unsigned int comparedEntityRefCollider     = linkedEntities[j];
				cm::transform* backtrackingTransform = componentManager->
					GetComponent<cm::transform>(backtrackingEntityRefCollider);
			    cm::transform* comparedTransform     = componentManager->
					GetComponent<cm::transform>(comparedEntityRefCollider);

				bool boxColliderFlag;
				bool upperActorCheckFlag = false;
                boxColliderFlag = BoxCollider(backtrackingTransform->tPosition,
											  comparedTransform->tPosition,
											  backtrackingTransform->fScale,
											  comparedTransform->fScale);
                upperActorCheckFlag = UpperActorCheck(backtrackingTransform->tPosition,
					                                  comparedTransform->tPosition,
					                                  backtrackingTransform->fScale,
													  comparedTransform->fScale);
				
				if(upperActorCheckFlag && boxColliderFlag) {
//					std::cout << "TEST 1" << std::endl;
                    componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->bGround_Collision_ = true;
                    componentManager->GetComponent<cm::collider>(comparedEntityRefCollider)->bGround_Collision_ = true;
                    continue;
                }
                    
                if(boxColliderFlag) {
//					std::cout << "TEST 2" << std::endl;
                    componentManager->GetComponent<cm::collider>(backtrackingEntityRefCollider)->bWall_Collision_ = true;
                    componentManager->GetComponent<cm::collider>(comparedEntityRefCollider)->bWall_Collision_ = true;
                    continue;
                }
//				std::cout << "No collissions" << std::endl;
			}
		}
	}

}
