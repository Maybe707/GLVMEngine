#ifndef GRAVITY_SYSTEM
#define GRAVITY_SYSTEM

#include "ComponentManager.hpp"
#include "Event.hpp"
#include "EventComponent.hpp"
#include "GravityComponent.hpp"
#include "ISystem.hpp"
#include "TransformComponent.hpp"
#include "VectorContainer.hpp"

namespace GLVM::ECS
{
    class CGravitySystem : public ECS::ISystem
    {
    public:
        float fAcceleration_of_Gravity_;
        
        bool BoxCollider(STransformComponent& _transform_Component1,
                         STransformComponent& _transform_Component2,
                         float& _fDelta_Time)
        {
            bool bCollision_Flag = false;
            
            if((_transform_Component1.tVertex[0] + 0.5f) > _transform_Component2.tVertex[0] &&
               _transform_Component1.tVertex[0] < _transform_Component2.tVertex[0] + 0.5f   &&
               (_transform_Component1.tVertex[1] + 0.5f) > _transform_Component2.tVertex[1] &&
               _transform_Component1.tVertex[1] < _transform_Component2.tVertex[1] + 0.5f   &&
               (_transform_Component1.tVertex[2] + 0.5f) > _transform_Component2.tVertex[2] &&
               _transform_Component1.tVertex[2] < _transform_Component2.tVertex[2] + 0.5f)
            {
//                fAcceleration_of_Gravity_ = 0.0f;
                bCollision_Flag           = true;
            }
            return bCollision_Flag;
        }

        void Gravity(STransformComponent& _transform_Component, SEventComponent& _event_Component, STransformComponent& _transform_Component2)
        {
            if(_event_Component.eEvent_ == Core::eGRAVITY_COLLISION_FLAG)
                _transform_Component.tVertex[1] = _transform_Component2.tVertex[1] + 0.5f;
            fAcceleration_of_Gravity_ = 0.0f;
//                _transform_Component.tVertex[1] += 0.01f;
        }
        
        void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override
        {
            int flag = 0;
            Core::TCVectorContainer<CGravityComponent>* _pGravity_Component_Container = ECS::GetInnerComponentContainer<ECS::CGravityComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* pOrdered_Move_Container                 =
                        GetInnerIDsContainer<SMoveComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* _pOrdered_Gravity_Container = ECS::GetInnerIDsContainer<ECS::CGravityComponent>(_Component_Manager);
            Core::TCVectorContainer<SEventComponent>* _pEvent_Component_Container = ECS::GetInnerComponentContainer<ECS::SEventComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* _pOrdered_Event_Container = ECS::GetInnerIDsContainer<ECS::SEventComponent>(_Component_Manager);
            Core::TCVectorContainer<STransformComponent>* _pTransform_Components_Container = ECS::GetInnerComponentContainer<ECS::STransformComponent>(_Component_Manager);
            // Core::TCVectorContainer<unsigned int>* _pOrdered_Move_Container = ECS::GetInnerIDsContainer<ECS::SMoveComponent>(_Component_Manager);
			// Core::TCVectorContainer<SMoveComponent>* _pMove_Components_Container = ECS::GetInnerComponentContainer<ECS::SMoveComponent>(_Component_Manager);
            Core::TCVectorContainer<ECS::CViewComponent>* _pViewContainer = ECS::GetInnerComponentContainer<ECS::CViewComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* _pOrdered_View_Container = ECS::GetInnerIDsContainer<ECS::CViewComponent>(_Component_Manager);
            Core::TCVectorContainer<SMoveComponent>* _pMove_Components_Container           =
            GetInnerComponentContainer<SMoveComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* pOrdered_Colliders_Container            =
                        GetInnerIDsContainer<CColliderComponent>(_Component_Manager);
            ECS::CViewComponent& view_Component = (*_pViewContainer)[(*_pOrdered_View_Container)[0]];  //!!!!!!!! REMOVE HARDCODE !!!!!!!!!!!

            for(int i = 0, iSize = pOrdered_Colliders_Container->GetSize(); i < iSize; ++i)
            {
                for(int j = 0,iSize_Iner = (pOrdered_Colliders_Container->GetSize()); j < iSize_Iner; ++j)
                {
                    if(i == j)
                        continue;
                    if(BoxCollider((*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[i]], (*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[j]], fAcceleration_of_Gravity_))
                        for(int x = 0, iSize_Rep = (_pOrdered_Gravity_Container->GetSize()); x < iSize_Rep; ++x)
                        {
                            if((*pOrdered_Colliders_Container)[i] == (*_pOrdered_Gravity_Container)[x])
                            {
                                std::cout << "Gravity" << std::endl;
                                Gravity((*_pTransform_Components_Container)[(*_pOrdered_Gravity_Container)[x]],
                                        (*_pEvent_Component_Container)[(*_pOrdered_Event_Container)[x]],
                                        (*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[j]]);
                            }
                        }
                }
            }
            
            for(int n = 0; n < _pOrdered_Gravity_Container->GetSize(); ++n)
            {
                _Component_Manager.GetComponent<ECS::STransformComponent>((*ECS::GetInnerIDsContainer<ECS::CGravityComponent>(_Component_Manager))[n]).tVertex[1] -= fAcceleration_of_Gravity_;
//                (*_pTransform_Components_Container)[(*_pOrdered_Gravity_Container)[n]].tVertex[1] -= fAcceleration_of_Gravity_;
                (*_pEvent_Component_Container)[(*_pOrdered_Event_Container)[n]].eEvent_ = Core::EEvents::eGRAVITY_COLLISION_FLAG;
            }
        }
    };
}

#endif
