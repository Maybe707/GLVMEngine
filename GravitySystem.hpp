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
        void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override
        {
            Core::TCVectorContainer<CGravityComponent>* _pGravity_Component_Container = ECS::GetInnerComponentContainer<ECS::CGravityComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* _pOrdered_Gravity_Container = ECS::GetInnerIDsContainer<ECS::CGravityComponent>(_Component_Manager);
            Core::TCVectorContainer<SEventComponent>* _pEvent_Component_Container = ECS::GetInnerComponentContainer<ECS::SEventComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* _pOrdered_Event_Container = ECS::GetInnerIDsContainer<ECS::SEventComponent>(_Component_Manager);
            Core::TCVectorContainer<STransformComponent>* _pTransform_Components_Container = ECS::GetInnerComponentContainer<ECS::STransformComponent>(_Component_Manager);
            // Core::TCVectorContainer<unsigned int>* _pOrdered_Move_Container = ECS::GetInnerIDsContainer<ECS::SMoveComponent>(_Component_Manager);
			// Core::TCVectorContainer<SMoveComponent>* _pMove_Components_Container = ECS::GetInnerComponentContainer<ECS::SMoveComponent>(_Component_Manager);
            Core::TCVectorContainer<ECS::CViewComponent>* _tViewContainer = ECS::GetInnerComponentContainer<ECS::CViewComponent>(_Component_Manager);
            Core::TCVectorContainer<unsigned int>* _pOrdered_View_Container = ECS::GetInnerIDsContainer<ECS::CViewComponent>(_Component_Manager);
            ECS::CViewComponent& view_Component = (*_tViewContainer)[(*_pOrdered_View_Container)[0]];  //!!!!!!!! REMOVE HARDCODE !!!!!!!!!!!
            for(int i = 0; i < _pOrdered_Gravity_Container->GetSize(); ++i)
            {
                (*_pTransform_Components_Container)[(*_pOrdered_Gravity_Container)[i]].tVertex[1] -= 0.01f;
                (*_pEvent_Component_Container)[(*_pOrdered_Event_Container)[i]].eEvent_ = Core::EEvents::eGRAVITY_COLLISION_FLAG;
            }
        }
    };
}

#endif
