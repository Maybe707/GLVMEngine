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
	// bool CCollisionSystem::BoxCollider(components::transform& _transform_Component1,
    //                                    components::transform& _transform_Component2)
	// {
    //     if(_transform_Component1.tPosition[0] + _transform_Component1.fScale / 2 > _transform_Component2.tPosition[0] - _transform_Component2.fScale / 2 &&
    //        _transform_Component1.tPosition[0] - _transform_Component1.fScale / 2 < _transform_Component2.tPosition[0] + _transform_Component2.fScale / 2 &&
    //        _transform_Component1.tPosition[1] + _transform_Component1.fScale / 2 > _transform_Component2.tPosition[1] - _transform_Component2.fScale / 2 &&
    //        _transform_Component1.tPosition[1] - _transform_Component1.fScale / 2 < _transform_Component2.tPosition[1] + _transform_Component2.fScale / 2 &&
    //        _transform_Component1.tPosition[2] + _transform_Component1.fScale / 2 > _transform_Component2.tPosition[2] - _transform_Component2.fScale / 2 &&
    //        _transform_Component1.tPosition[2] - _transform_Component1.fScale / 2 < _transform_Component2.tPosition[2] + _transform_Component2.fScale / 2)
    //     {
    //         return true;
    //     }
        
	// 	return false;
	// }

    // bool CCollisionSystem::UpperActorCheck(components::transform& _transform_Component1,
    //                                        components::transform& _transform_Component2) {
    //     if((_transform_Component1.tPosition[1] - _transform_Component1.fScale / 2) > (_transform_Component2.tPosition[1] + (_transform_Component2.fScale / 2  - (_transform_Component2.fScale / 10)))) {
    //         return true;
    //     }

    //     return false;
    // }

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
        if((backtrackingPosition[1] - backtrackingScale / 2) > (comparedPosition[1] + (comparedScale / 2  - (comparedScale / 10)))) {
            return true;
        }

        return false;
    }

	
	void CCollisionSystem::Update()
	{
		namespace cm = GLVM::ecs::components;
		
        CComponentManager* componentManager = CComponentManager::GetInstance();
        core::vector<unsigned int>* entityContainerRefCollider =
			componentManager->GetEntityContainer<cm::collider>();
        unsigned int vectorColliderSize = entityContainerRefCollider->GetSize();

		for(unsigned int i = 0; i < vectorColliderSize; ++i) {
			for(unsigned int j = i + 1; j < vectorColliderSize; ++j) {
                unsigned int backtrackingEntityRefCollider = (*entityContainerRefCollider)[i];  
                unsigned int comparedEntityRefCollider     = (*entityContainerRefCollider)[j];
				cm::transform* backtrackingTransform = componentManager->GetComponent<cm::transform>(backtrackingEntityRefCollider);
			    cm::transform* comparedTransform     = componentManager->GetComponent<cm::transform>(comparedEntityRefCollider);
				componentManager->CreateComponent<cm::move>(backtrackingEntityRefCollider);
				componentManager->CreateComponent<cm::move>(comparedEntityRefCollider);
				cm::move* backtrackingMove           = componentManager->GetComponent<cm::move>(backtrackingEntityRefCollider);
			    cm::move* comparedMove               = componentManager->GetComponent<cm::move>(comparedEntityRefCollider);

				bool boxColliderFlag;
				bool upperActorCheckFlag = false;;
				if ( comparedTransform != nullptr && backtrackingTransform != nullptr
					&& comparedMove != nullptr && backtrackingMove != nullptr) {
                boxColliderFlag = BoxCollider(backtrackingTransform->tPosition + backtrackingMove->frameMovement,
					                                  comparedTransform->tPosition + comparedMove->frameMovement,
					                                  backtrackingTransform->fScale, comparedTransform->fScale);
                upperActorCheckFlag = UpperActorCheck(backtrackingTransform->tPosition + backtrackingMove->frameMovement,
					                                  comparedTransform->tPosition + comparedMove->frameMovement,
					                                  backtrackingTransform->fScale, comparedTransform->fScale);
				}
				
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
			}
		}
//		std::cout << "Size: " << componentManager->GetComponentContainer<cm::move>()->GetSize() << std::endl;
	}

}
