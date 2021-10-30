#include "MatrixComponent.hpp"

namespace GLVM::ECS
{
	
	CMatrixComponent::CMatrixComponent(double _fScale)
	{
		for(int count = 0; count < (u_iRange-LIMITER); ++count)
		{
			aMatrix_[count][count] = _fScale;
		}
		aMatrix_[u_iRange-LIMITER][u_iRange-LIMITER] = HOMOGENEOUS_COORDINATE;
	}

	float* CMatrixComponent::GetMatrix()
	{
		return *aMatrix_;
	}

	float (&CMatrixComponent::Matrix())[u_iRange][u_iRange]
	{
		return aMatrix_;
	}

	void CMatrixComponent::Offset(const STransformComponent& _transform_Component)
	{
		aMatrix_[u_iRange-LIMITER][0] = _transform_Component.fPos_X;
		aMatrix_[u_iRange-LIMITER][1] = _transform_Component.fPos_Y;
		aMatrix_[u_iRange-LIMITER][2] = _transform_Component.fPos_Z;
	}

	void CMatrixComponent::Move(double _dOffset, Core::CEvent _Event)
	{
		switch(_Event.GetEvent())
		{
		case GLVM::Core::EEvents::eMOVE_LEFT:
			aMatrix_[u_iRange-LIMITER][OFFSET_X] -= _dOffset;
			break;
		case GLVM::Core::EEvents::eMOVE_RIGHT:
			aMatrix_[u_iRange-LIMITER][OFFSET_X] += _dOffset;
			break;
		case GLVM::Core::EEvents::eMOVE_DOWN:
			aMatrix_[u_iRange-LIMITER][OFFSET_Y] -= _dOffset;
			break;
		case GLVM::Core::EEvents::eMOVE_UP:
			aMatrix_[u_iRange-LIMITER][OFFSET_Y] += _dOffset;
			break;
		}
	}
}
