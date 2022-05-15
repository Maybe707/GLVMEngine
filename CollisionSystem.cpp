#include "CollisionSystem.hpp"
#include "Event.hpp"
#include "EventComponent.hpp"
#include "GravityComponent.hpp"
#include "ViewComponent.hpp"

namespace GLVM::ECS
{
	void CCollisionSystem::Repel(STransformComponent& _transform_Component,
                                 SMoveComponent& _move_Component,
                                 float& _fDelta_Time,
                                 CViewComponent& _view_Component,
                                 SEventComponent& _event_Component)
	{
        // switch(_move_Component.eEvent_)
        // {
        // case Core::eMOVE_UP:
        //     _transform_Component.tVertex[2] = (_transform_Component.tVertex[2] - _fDelta_Time);
        // case Core::eMOVE_DOWN:
        //     _transform_Component.tVertex[2] = (_transform_Component.tVertex[2] + _fDelta_Time);
        // case Core::eMOVE_RIGHT:
        //     _transform_Component.tVertex[0] = (_transform_Component.tVertex[0] - _fDelta_Time);
        // case Core::eMOVE_LEFT:
        //     _transform_Component.tVertex[0] = (_transform_Component.tVertex[0] + _fDelta_Time);
        // default:
        //     break;
        // }
        
        // if(_event_Component.eEvent_ == Core::eGRAVITY_COLLISION_FLAG)
        // {
        //     _transform_Component.tVertex[1] += 0.01f;
        //     return;
        // }
        
        float cameraSpeed = static_cast<float>(2.5 * _fDelta_Time);
        switch(_move_Component.eEvent_)
        {
        case Core::eMOVE_UP:
            _transform_Component.tVertex -= _view_Component.Front_Camera * cameraSpeed;
            break;
        case Core::eMOVE_DOWN:
            _transform_Component.tVertex += _view_Component.Front_Camera * cameraSpeed;
            break;
        case Core::eMOVE_RIGHT:
            _transform_Component.tVertex -= Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera)) * cameraSpeed;
            break;
        case Core::eMOVE_LEFT:
            _transform_Component.tVertex += Normalize(Cross(_view_Component.Front_Camera, _view_Component.Up_Camera)) * cameraSpeed;
            break;
        default:
            break;
        }
	}

    bool CCollisionSystem::Gravity(STransformComponent& _transform_Component, SEventComponent& _event_Component)
    {
        if(_event_Component.eEvent_ == Core::eGRAVITY_COLLISION_FLAG)
        {
            _transform_Component.tVertex[1] += 0.01f;
            return false;
        }
        
        return true;
    }
    
	bool CCollisionSystem::BoxCollider(STransformComponent& _transform_Component1,
                                       STransformComponent& _transform_Component2,
                                       SEventComponent& _event_Component,
                                       float& _fDelta_Time)
	{
		bool bCollision_Flag = false;

        if(_transform_Component1.tVertex[1] > (_transform_Component2.tVertex[1] + 0.45f))
        {
            bCollision_Flag = false;
            return bCollision_Flag;
        }
        
        if((_transform_Component1.tVertex[0] + 0.5f) > _transform_Component2.tVertex[0] &&
           _transform_Component1.tVertex[0] < _transform_Component2.tVertex[0] + 0.5f   &&
           (_transform_Component1.tVertex[1] + 0.5f) > _transform_Component2.tVertex[1] &&
           _transform_Component1.tVertex[1] < _transform_Component2.tVertex[1] + 0.5f   &&
           (_transform_Component1.tVertex[2] + 0.5f) > _transform_Component2.tVertex[2] &&
           _transform_Component1.tVertex[2] < _transform_Component2.tVertex[2] + 0.5f)
        {
            bCollision_Flag = true;
        }
		return bCollision_Flag;
	}

	void CCollisionSystem::Update(CComponentManager& _Component_Manager, Core::CEvent& _Event)
	{
		Core::TCVectorContainer<STransformComponent>* _pTransform_Components_Container =
            GetInnerComponentContainer<STransformComponent>(_Component_Manager);
  		Core::TCVectorContainer<unsigned int>* pOrdered_Colliders_Container            =
            GetInnerIDsContainer<CColliderComponent>(_Component_Manager);
		Core::TCVectorContainer<SMoveComponent>* _pMove_Components_Container           =
            GetInnerComponentContainer<SMoveComponent>(_Component_Manager);
		Core::TCVectorContainer<unsigned int>* pOrdered_Move_Container                 =
            GetInnerIDsContainer<SMoveComponent>(_Component_Manager);
        Core::TCVectorContainer<SEventComponent>* _pEvent_Component_Container          =
            GetInnerComponentContainer<ECS::SEventComponent>(_Component_Manager);
        Core::TCVectorContainer<unsigned int>* _pOrdered_Event_Container               =
            GetInnerIDsContainer<ECS::SEventComponent>(_Component_Manager);
        Core::TCVectorContainer<ECS::CViewComponent>* _pViewContainer                  =
            GetInnerComponentContainer<ECS::CViewComponent>(_Component_Manager);
        Core::TCVectorContainer<unsigned int>* _pOrdered_View_Container                =
            GetInnerIDsContainer<ECS::CViewComponent>(_Component_Manager);
        ECS::CViewComponent& view_Component = (*_pViewContainer)[(*_pOrdered_View_Container)[0]];
		for(int i = 0, iSize = pOrdered_Colliders_Container->GetSize(); i < iSize; ++i)
		{
			for(int j = 0,iSize_Iner = (pOrdered_Colliders_Container->GetSize()); j < iSize_Iner; ++j)
			{
				if(i == j)
					continue;
				if(BoxCollider((*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[i]], (*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[j]], (*_pEvent_Component_Container)[(*_pOrdered_Event_Container)[i]], fDelta_Time_))
					for(int x = 0, iSize_Rep = (pOrdered_Move_Container->GetSize()); x < iSize_Rep; ++x)
                    {
                        if((*pOrdered_Colliders_Container)[i] == (*pOrdered_Move_Container)[x] &&
                           (*pOrdered_Colliders_Container)[i] == (*_pOrdered_View_Container)[x])
                        {
							Repel((*_pTransform_Components_Container)[(*pOrdered_Move_Container)[x]],
                                  (*_pMove_Components_Container)[(*pOrdered_Move_Container)[x]], fDelta_Time_,
                                  (*_pViewContainer)[(*_pOrdered_View_Container)[x]],
                                (*_pEvent_Component_Container)[(*_pOrdered_Event_Container)[x]]);
                        }
                    }
			}
		}
	}

}
