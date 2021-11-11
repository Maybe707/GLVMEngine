#include "CollisionSystem.hpp"

namespace GLVM::ECS
{
	void CCollisionSystem::Repel(STransformComponent& _transform_Component, SMoveComponent& _move_Component, double& _fDelta_Time)
	{
		if (_move_Component.eEvent_ == Core::eMOVE_UP)
		{
			_transform_Component.fPos_Y = (_transform_Component.fPos_Y - _fDelta_Time);
		}
		if (_move_Component.eEvent_ == Core::eMOVE_DOWN)
		{
			_transform_Component.fPos_Y = (_transform_Component.fPos_Y + _fDelta_Time);
		}
		if (_move_Component.eEvent_ == Core::eMOVE_RIGHT)
		{
		    _transform_Component.fPos_X = (_transform_Component.fPos_X - _fDelta_Time);
		}
		if (_move_Component.eEvent_ == Core::eMOVE_LEFT)
		{
			_transform_Component.fPos_X = (_transform_Component.fPos_X + _fDelta_Time);
		}
	}

	bool CCollisionSystem::BoxCollider(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2)
	{
		bool bCollision_Flag = false;
		float fX = 0;
		float fY = 0;
		fX = std::abs(_transform_Component2.fPos_X - _transform_Component1.fPos_X);
		fY = std::abs(_transform_Component2.fPos_Y - _transform_Component1.fPos_Y);
		if(fX < 32.0f && fY < 32.0f)
			bCollision_Flag = true;

		return bCollision_Flag;
	}

	void CCollisionSystem::Update(CComponentManager& _Component_Manager)
	{
		Core::TCConstVectorContainer<STransformComponent>* _pTransform_Components_Container = GetInnerMainContainer<STransformComponent>(_Component_Manager);
		Core::TCVectorContainer<unsigned int>* pOrdered_Colliders_Container = GetInnerIndexContainer<CColliderComponent>(_Component_Manager);
		Core::TCConstVectorContainer<SMoveComponent>* _pMove_Components_Container = GetInnerMainContainer<SMoveComponent>(_Component_Manager);
		Core::TCVectorContainer<unsigned int>* pOrdered_Move_Container = GetInnerIndexContainer<SMoveComponent>(_Component_Manager);
		for(int i = 0, iSize = pOrdered_Colliders_Container->GetSize(); i < iSize; ++i)
		{
			for(int j = 0,iSize_Iner = (pOrdered_Colliders_Container->GetSize()); j < iSize_Iner; ++j)
			{
				if(i == j)
					continue;
				if(BoxCollider((*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[i]], (*_pTransform_Components_Container)[(*pOrdered_Colliders_Container)[j]]))
					for(int x = 0, iSize_Rep = (pOrdered_Move_Container->GetSize()); x < iSize_Rep; ++x)
						if((*pOrdered_Colliders_Container)[i] == (*pOrdered_Move_Container)[x])
							Repel((*_pTransform_Components_Container)[(*pOrdered_Move_Container)[x]], (*_pMove_Components_Container)[(*pOrdered_Move_Container)[x]], _dDelta_Time);
			}
		}
	}

}
