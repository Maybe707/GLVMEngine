#ifndef MATRIX
#define MATRIX

#include "Event.hpp"
#include "VertexVector.hpp"

#define LIMITER 1
#define HOMOGENEOUS_COORDINATE 1
#define OFFSET_X 0
#define OFFSET_Y 1

namespace GLVM::Math
{
	template <unsigned int u_iRange>
	class TCMatrix
	{
		float aMatrix_[u_iRange][u_iRange] {};
	public:
		TCMatrix(double _fScale = 0.1)
		{
			for(int count = 0; count < (u_iRange-LIMITER); ++count)
			{
				aMatrix_[count][count] = _fScale;
			}
			aMatrix_[u_iRange-LIMITER][u_iRange-LIMITER] = HOMOGENEOUS_COORDINATE;
		}

		float* GetMatrix()
		{
			return *aMatrix_;
		}

		float (&Matrix())[u_iRange][u_iRange]
		{
			return aMatrix_;
		}
		
		void Offset(const Core::SVertexVector& _VertexVector)
		{
			aMatrix_[u_iRange-LIMITER][0] = _VertexVector.fAxis_X;
			aMatrix_[u_iRange-LIMITER][1] = _VertexVector.fAxis_Y;
			aMatrix_[u_iRange-LIMITER][2] = _VertexVector.fAxis_Z;
		}

		void Move(double _dOffset, Core::CEvent _Event)
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
	};
}

#endif
