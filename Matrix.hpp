#ifndef MATRIX
#define MATRIX

#include "Event.hpp"
#include "VertexVector.hpp"
#include "ConstVectorContainer.hpp"

#define LIMITER 1
#define HOMOGENEOUS_COORDINATE 1
#define OFFSET_X 0
#define OFFSET_Y 1

namespace GLVM::Math
{
	template <unsigned int u_iRange>
	class TCMatrix
	{
		static Core::TCConstVectorContainer<TCMatrix<u_iRange>*> s_tMatrix_Components_Array;
		float aMatrix_[u_iRange][u_iRange] {};
	public:
		TCMatrix(double _fScale = 0.1, const unsigned int _Entity_ID = 0);

		float* GetMatrix();
		float (&Matrix())[u_iRange][u_iRange];
		void Offset(const Core::SVertexVector& _VertexVector);
		void Move(double _dOffset, Core::CEvent _Event);
		Core::TCConstVectorContainer<TCMatrix<u_iRange>*>& GetMatrixComponentsArray();
	};

	template<unsigned int u_iRange>
	TCMatrix<u_iRange>::TCMatrix(double _fScale, const unsigned int _Entity_ID)
	{
		s_tMatrix_Components_Array.Push(this, _Entity_ID);
		for(int count = 0; count < (u_iRange-LIMITER); ++count)
		{
			aMatrix_[count][count] = _fScale;
		}
		aMatrix_[u_iRange-LIMITER][u_iRange-LIMITER] = HOMOGENEOUS_COORDINATE;
	}

	template<unsigned int u_iRange>
	float* TCMatrix<u_iRange>::GetMatrix()
	{
		return *aMatrix_;
	}

	template<unsigned int u_iRange>
	float (&TCMatrix<u_iRange>::Matrix())[u_iRange][u_iRange]
	{
		return aMatrix_;
	}

	template<unsigned int u_iRange>
	void TCMatrix<u_iRange>::Offset(const Core::SVertexVector& _VertexVector)
	{
		aMatrix_[u_iRange-LIMITER][0] = _VertexVector.fAxis_X;
		aMatrix_[u_iRange-LIMITER][1] = _VertexVector.fAxis_Y;
		aMatrix_[u_iRange-LIMITER][2] = _VertexVector.fAxis_Z;
	}

	template<unsigned int u_iRange>
	void TCMatrix<u_iRange>::Move(double _dOffset, Core::CEvent _Event)
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

	template<unsigned int u_iRange>
	Core::TCConstVectorContainer<TCMatrix<u_iRange>*>&
	TCMatrix<u_iRange>::GetMatrixComponentsArray()
	{
		return s_tMatrix_Components_Array;
	}
}

#endif
