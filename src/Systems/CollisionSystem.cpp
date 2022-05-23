#include "Systems/CollisionSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Event.hpp"
#include "Components/EventComponent.hpp"
#include "Components/GravityComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Stack.hpp"
#include "VectorContainer.hpp"

namespace GLVM::ECS
{
	bool CCollisionSystem::BoxCollider(STransformComponent& _transform_Component1,
                                       STransformComponent& _transform_Component2)
	{
        if(_transform_Component1.tVertex[0] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[0] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[0] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[0] + _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[1] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[1] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[1] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[1] + _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[2] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[2] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tVertex[2] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[2] + _transform_Component2.fScale / 2)
        {
            return true;
        }
        
		return false;
	}

    bool CCollisionSystem::UpperActorCheck(STransformComponent& _transform_Component1,
                                           STransformComponent& _transform_Component2)
    {
        if((_transform_Component1.tVertex[1] - _transform_Component1.fScale / 2) > (_transform_Component2.tVertex[1] + (_transform_Component2.fScale / 2  - 0.2f)))
        {
            return true;
        }

        return false;
    }
    
	void CCollisionSystem::Update(CComponentManager& _Component_Manager, Core::CEvent& _Event)
	{
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refCollider =
            ECS::GetInnerIDsContainer<ECS::CColliderComponent>(_Component_Manager);
        unsigned int uiVector_Collider_Size = pEntity_Container_refCollider->GetSize();
            
		for(int i = 0, iSize = uiVector_Collider_Size; i < iSize; ++i)
		{

//            _Component_Manager.GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).aColliders_Container_[uiBacktracking_Entity_refCollider].bGround_Collision_ = false;
			for(int j = 0, iSize_Iner = uiVector_Collider_Size; j < iSize_Iner; ++j)
			{
                unsigned int uiBacktracking_Entity_refCollider = (*pEntity_Container_refCollider)[i];  
                unsigned int uiCompared_Entity_refCollider = (*pEntity_Container_refCollider)[j];

				if(uiBacktracking_Entity_refCollider == uiCompared_Entity_refCollider)
					continue;

                // if(UpperActorCheck(_Component_Manager.GetComponent<ECS::STransformComponent>(uiBacktracking_Entity_refCollider),
                //                _Component_Manager.GetComponent<ECS::STransformComponent>(uiCompared_Entity_refCollider)))
                //     std::cout << "Collision TRUE!!!" << std::endl;
                // else
                //     std::cout << "Collision FALSE!!!" << std::endl;

//                std::cout << "J: " << uiCompared_Entity_refCollider << std::endl;
                
                bool bBox_Collider_Flag = BoxCollider(_Component_Manager.GetComponent<ECS::STransformComponent>(uiBacktracking_Entity_refCollider),
                                                      _Component_Manager.GetComponent<ECS::STransformComponent>(uiCompared_Entity_refCollider));
                bool bUpper_Actor_Check_Flag = UpperActorCheck(_Component_Manager.GetComponent<ECS::STransformComponent>(uiBacktracking_Entity_refCollider), _Component_Manager.GetComponent<ECS::STransformComponent>(uiCompared_Entity_refCollider));

                if(_Component_Manager.GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).aColliders_Container_[uiBacktracking_Entity_refCollider].bGround_Collision_)
                    std::cout << "TRUE! " << uiBacktracking_Entity_refCollider << std::endl;

//                _Component_Manager.GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).aColliders_Container_[uiBacktracking_Entity_refCollider].bGround_Collision_ = false;
                
                // if(bUpper_Actor_Check_Flag)
                //     std::cout << "i and j: " << i << " " << j << std::endl;

				if(bUpper_Actor_Check_Flag && bBox_Collider_Flag)
                {
                    std::cout << "i and j: " << i << " " << j << std::endl;
                    _Component_Manager.GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).aColliders_Container_[uiBacktracking_Entity_refCollider].bGround_Collision_ = true;
                    _Component_Manager.GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).aColliders_Container_[uiBacktracking_Entity_refCollider].uiGround_Collider_ = uiCompared_Entity_refCollider;
                    continue;
                } else {
//                    _Component_Manager.GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).aColliders_Container_[uiBacktracking_Entity_refCollider].bGround_Collision_ = false;
                }
               
                if(bBox_Collider_Flag)
                {
//                    std::cout << "Flag!" << std::endl;
                    _Component_Manager.GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).aColliders_Container_[uiBacktracking_Entity_refCollider].bWall_Collision_ = true;
                    _Component_Manager.GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).aColliders_Container_[uiBacktracking_Entity_refCollider].uiWall_Collider_ = uiCompared_Entity_refCollider;
                    continue;
                }
			}
		}
	}

}
