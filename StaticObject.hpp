#ifndef STATIC_OBJECT
#define STATIC_OBJECT

#include "GameObject.hpp"
#include "Texture.hpp"
#include "Matrix.hpp"

#define MATRIX_RANGE 4

namespace GLVM::Core 
{
    
	class CStaticObject : public IGameObject
	{
		CTexture Texture_;
		Math::TCMatrix<MATRIX_RANGE> tModel_Matrix_;
	public:
		CStaticObject(const CTexture& _Texture) : Texture_(_Texture) {}
		
		virtual CTexture& GetTexture() override
		{
			return Texture_;
		}

		virtual Math::TCMatrix<MATRIX_RANGE>* GetMatrix() override
		{
			return &tModel_Matrix_;
		}
	};

}

#endif
