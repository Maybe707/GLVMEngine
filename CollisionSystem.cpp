#include "CollisionSystem.hpp"
#include "ViewComponent.hpp"

namespace GLVM::ECS
{
	void CCollisionSystem::Repel(STransformComponent& _transform_Component, SMoveComponent& _move_Component,
                                 double& _fDelta_Time, CViewComponent& _view_Component)
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

	bool CCollisionSystem::BoxCollider(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2)
	{
		bool bCollision_Flag = false;
		float fX = 0;
		float fY = 0;
        float fZ = 0;
		fX = std::abs(_transform_Component2.tVertex[0] - _transform_Component1.tVertex[0]);
		fY = std::abs(_transform_Component2.tVertex[1] - _transform_Component1.tVertex[1]);
        fZ = std::abs(_transform_Component2.tVertex[2] - _transform_Component1.tVertex[2]);
		if(fX < 0.5f && fY < 0.5f && fZ < 0.5f)
			bCollision_Flag = true;

		return bCollision_Flag;
	}

	void CCollisionSystem::Update(CComponentManager& _Component_Manager, Core::CEvent& _Event)
	{
		Core::TCVectorContainer<STransformComponent>* _pTransform_Components_Container = GetInnerComponentContainer<STransformComponent>(_Component_Manager);
		Core::TCVectorContainer<unsigned int>* pOrdered_Colliders_Container = GetInnerIDsContainer<CColliderComponent>(_Component_Manager);
		Core::TCVectorContainer<SMoveComponent>* _pMove_Components_Container = GetInnerComponentContainer<SMoveComponent>(_Component_Manager);
		Core::TCVectorContainer<unsigned int>* pOrdered_Move_Container = GetInnerIDsContainer<SMoveComponent>(_Component_Manager);

        Core::TCVectorContainer<ECS::CViewComponent>* _pViewContainer = ECS::GetInnerComponentContainer<ECS::CViewComponent>(_Component_Manager);
        Core::TCVectorContainer<unsigned int>* _pOrdered_View_Container = ECS::GetInnerIDsContainer<ECS::CViewComponent>(_Component_Manager);
        ECS::CViewComponent& view_Component = (*_pViewContainer)[(*_pOrdered_View_Container)[0]];
		for(int i = 0, iSize = pOrdered_Colliders_Container->GetSize(); i < iSize; ++i)
		{
			for(int j = 0,iSize_Iner = (pOrdered_Colliders_Container->GetSize()); j < iSize_Iner; ++j)
			{
				if(i == j)
					continue;
				if(BoxCollider((*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[i]], (*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[j]]))
					for(int x = 0, iSize_Rep = (pOrdered_Move_Container->GetSize()); x < iSize_Rep; ++x)
                    {
						// if((*pOrdered_Colliders_Container)[i] == (*pOrdered_Move_Container)[x])
						// 	Repel((*_pTransform_Components_Container)[(*pOrdered_Move_Container)[x]], (*_pMove_Components_Container)[(*pOrdered_Move_Container)[x]], _dDelta_Time);
                        if((*pOrdered_Colliders_Container)[i] == (*pOrdered_Move_Container)[x] &&
                           (*pOrdered_Colliders_Container)[i] == (*_pOrdered_View_Container)[x])
                        {
							Repel((*_pTransform_Components_Container)[(*pOrdered_Move_Container)[x]],
                                  (*_pMove_Components_Container)[(*pOrdered_Move_Container)[x]], _dDelta_Time
                                  , (*_pViewContainer)[(*_pOrdered_View_Container)[x]]);
                        }
                    }
			}
		}
	}

}
