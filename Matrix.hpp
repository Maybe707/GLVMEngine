#ifndef MATRIX
#define MATRIX

#define LIMITER 1
#define HOMOGENEOUS_COORDINATE 1
#define OFFSET_X 0
#define OFFSET_Y 1

#include "Event.hpp"

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
		
		void Offset(double _fOffset)
		{
			for(int count = 0; count < (u_iRange-LIMITER); ++count)
			{
				aMatrix_[u_iRange-LIMITER][count] = _fOffset;
			}
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
