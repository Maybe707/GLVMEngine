#include "Matrix.hpp"
#include "Texture.hpp"

#define MATRIX_SIZE 4

namespace GLVM::Core
{
	class CPlayer
	{
		CTexture Texture_;
		Math::TCMatrix<MATRIX_SIZE> tModel_Matrix_;
	public:
		void SetTexture(const CTexture& _Texture)
		{
			Texture_ = _Texture;
		}
		
	};
}
