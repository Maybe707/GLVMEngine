#ifndef STATIC_OBJECT
#define STATIC_OBJECT

#include "GameObject.hpp"
#include "Texture.hpp"
#include "Matrix.hpp"
#include "VertexVector.hpp"

#define MATRIX_RANGE 4

namespace GLVM::Core 
{
    
	class CStaticObject : public IGameObject
	{
		SVertexVector vertex_Vector_;
		const char kSign_ = 's';
		CTexture Texture_;
		Math::TCMatrix<MATRIX_RANGE> tModel_Matrix_;
	public:
		CStaticObject(const CTexture& _Texture);
		virtual SVertexVector& GetVertexVector() override;
		virtual const char& GetSign() override;
		virtual CTexture& GetTexture() override;
		void SetMatrix(Math::TCMatrix<MATRIX_RANGE>& _tModel_Matrix);
		virtual Math::TCMatrix<MATRIX_RANGE>* GetMatrix() override;
	};

}

#endif
