#include "Systems/CollisionSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Event.hpp"
#include "Components/EventComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
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
    
	void CCollisionSystem::Update()
	{
        CComponentManager* _Component_Manager = CComponentManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refCollider =
            ECS::GetInnerIDsContainer<ECS::CColliderComponent>(*_Component_Manager);
        unsigned int uiVector_Collider_Size = pEntity_Container_refCollider->GetSize();
//        std::cout << "Vector size: " << uiVector_Collider_Size << std::endl;
		for(int i = 0, iSize = uiVector_Collider_Size; i < iSize; ++i)
		{
			for(int j = 0, iSize_Iner = uiVector_Collider_Size; j < iSize_Iner; ++j)
			{
                unsigned int uiBacktracking_Entity_refCollider = (*pEntity_Container_refCollider)[i];  
                unsigned int uiCompared_Entity_refCollider = (*pEntity_Container_refCollider)[j];

                if(uiBacktracking_Entity_refCollider == uiCompared_Entity_refCollider)
                    continue;
                
                bool bBox_Collider_Flag = BoxCollider(_Component_Manager->GetComponent<ECS::STransformComponent>(uiBacktracking_Entity_refCollider),
                                                      _Component_Manager->GetComponent<ECS::STransformComponent>(uiCompared_Entity_refCollider));
                bool bUpper_Actor_Check_Flag = UpperActorCheck(_Component_Manager->GetComponent<ECS::STransformComponent>(uiBacktracking_Entity_refCollider), _Component_Manager->GetComponent<ECS::STransformComponent>(uiCompared_Entity_refCollider));

				if(bUpper_Actor_Check_Flag && bBox_Collider_Flag)
                {
                    _Component_Manager->GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).bGround_Collision_ = true;
                    _Component_Manager->GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).uiGround_Collider_ = uiCompared_Entity_refCollider;
                    continue;
                }
                    
                if(bBox_Collider_Flag)
                {
                    _Component_Manager->GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).bWall_Collision_ = true;
                    _Component_Manager->GetComponent<ECS::CColliderComponent>(uiBacktracking_Entity_refCollider).uiWall_Collider_ = uiCompared_Entity_refCollider;
                    continue;
                }
			}
		}
	}

}
