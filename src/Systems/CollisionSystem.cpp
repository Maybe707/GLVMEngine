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
#include "VectorContainer.hpp"

namespace GLVM::ECS
{
	bool CCollisionSystem::BoxCollider(transform& _transform_Component1,
                                       transform& _transform_Component2)
	{
        if(_transform_Component1.tPosition[0] + _transform_Component1.fScale / 2 > _transform_Component2.tPosition[0] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tPosition[0] - _transform_Component1.fScale / 2 < _transform_Component2.tPosition[0] + _transform_Component2.fScale / 2 &&
           _transform_Component1.tPosition[1] + _transform_Component1.fScale / 2 > _transform_Component2.tPosition[1] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tPosition[1] - _transform_Component1.fScale / 2 < _transform_Component2.tPosition[1] + _transform_Component2.fScale / 2 &&
           _transform_Component1.tPosition[2] + _transform_Component1.fScale / 2 > _transform_Component2.tPosition[2] - _transform_Component2.fScale / 2 &&
           _transform_Component1.tPosition[2] - _transform_Component1.fScale / 2 < _transform_Component2.tPosition[2] + _transform_Component2.fScale / 2)
        {
            return true;
        }
        
		return false;
	}

    bool CCollisionSystem::UpperActorCheck(transform& _transform_Component1,
                                           transform& _transform_Component2)
    {
        if((_transform_Component1.tPosition[1] - _transform_Component1.fScale / 2) > (_transform_Component2.tPosition[1] + (_transform_Component2.fScale / 2  - 0.2f)))
        {
            return true;
        }

        return false;
    }
    
	void CCollisionSystem::Update()
	{
        CComponentManager* _Component_Manager = CComponentManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refCollider =
            ECS::GetInnerIDsContainer<ECS::collider>(*_Component_Manager);
        unsigned int uiVector_Collider_Size = pEntity_Container_refCollider->GetSize();
//        std::cout << "Vector size: " << uiVector_Collider_Size << std::endl;
        //std::cout << "Entities: " << Entity_Manager_Size << std::endl;
        
		for(int i = 0, iSize = uiVector_Collider_Size; i < iSize; ++i)
		{
			for(int j = i + 1, iSize_Iner = uiVector_Collider_Size; j < iSize_Iner; ++j)
			{
                unsigned int uiBacktracking_Entity_refCollider = (*pEntity_Container_refCollider)[i];  
                unsigned int uiCompared_Entity_refCollider = (*pEntity_Container_refCollider)[j];

                // if(uiBacktracking_Entity_refCollider == uiCompared_Entity_refCollider)
                //     continue;
                // if(uiCompared_Entity_refCollider == 0)
                //     continue;
                
                bool bBox_Collider_Flag = BoxCollider(_Component_Manager->GetComponent<ECS::transform>(uiBacktracking_Entity_refCollider),
                                                      _Component_Manager->GetComponent<ECS::transform>(uiCompared_Entity_refCollider));
                bool bUpper_Actor_Check_Flag = UpperActorCheck(_Component_Manager->GetComponent<ECS::transform>(uiBacktracking_Entity_refCollider), _Component_Manager->GetComponent<ECS::transform>(uiCompared_Entity_refCollider));

				if(bUpper_Actor_Check_Flag && bBox_Collider_Flag)
                {
                    _Component_Manager->GetComponent<ECS::collider>(uiBacktracking_Entity_refCollider).bGround_Collision_ = true;
                    _Component_Manager->GetComponent<ECS::collider>(uiCompared_Entity_refCollider).bGround_Collision_ = true;
                    _Component_Manager->GetComponent<ECS::collider>(uiBacktracking_Entity_refCollider).uiGround_Collider_ = uiCompared_Entity_refCollider;
                    continue;
                }
                    
                if(bBox_Collider_Flag)
                {
                    _Component_Manager->GetComponent<ECS::collider>(uiBacktracking_Entity_refCollider).bWall_Collision_ = true;
                    _Component_Manager->GetComponent<ECS::collider>(uiCompared_Entity_refCollider).bWall_Collision_ = true;
                    _Component_Manager->GetComponent<ECS::collider>(uiBacktracking_Entity_refCollider).uiWall_Collider_ = uiCompared_Entity_refCollider;
                    continue;
                }
			}
		}
	}

}
