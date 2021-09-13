#ifndef MATRIX
#define MATRIX

#define LIMITER 1
#define HOMOGENEOUS_COORDINATE 1

namespace GLVM::Math
{
	template <unsigned int uiRange>
	class TCMatrix
	{
		float aMatrix_[uiRange][uiRange] {};
	public:
		TCMatrix(float _fScale)
		{
			for(int count = 0; count < (uiRange-LIMITER); ++count)
			{
				aMatrix_[count][count] = _fScale;
			}
			aMatrix_[uiRange-LIMITER][uiRange-LIMITER] = HOMOGENEOUS_COORDINATE;
		}

		float const* GetMatrix() const
		{
			return *aMatrix_;
		}

		void Offset(float _fOffset)
		{
			for(int count = 0; count < (uiRange-LIMITER); ++count)
			{
				aMatrix_[uiRange-LIMITER][count] = _fOffset;
			}
		}
	};
}

#endif
