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
		CStaticObject(const CTexture& _Texture) : Texture_(_Texture)
		{
			vertex_Vector_.fAxis_X = tModel_Matrix_.Matrix()[3][0];
			vertex_Vector_.fAxis_Y = tModel_Matrix_.Matrix()[3][1];
		}

		virtual SVertexVector& GetVertexVector() override { return vertex_Vector_; }
		virtual const char& GetSign() override { return kSign_; }
		virtual CTexture& GetTexture() override { return Texture_; }
		void SetMatrix(Math::TCMatrix<MATRIX_RANGE>& _tModel_Matrix)
		{
			tModel_Matrix_ = _tModel_Matrix;
			vertex_Vector_.fAxis_X = tModel_Matrix_.Matrix()[3][0];
			vertex_Vector_.fAxis_Y = tModel_Matrix_.Matrix()[3][1];
		}
		virtual Math::TCMatrix<MATRIX_RANGE>* GetMatrix() override { return &tModel_Matrix_; }
	};

}

#endif
