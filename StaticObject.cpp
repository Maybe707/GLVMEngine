#include "StaticObject.hpp"

namespace GLVM::Core
{

	CStaticObject::CStaticObject(const CTexture& _Texture) : Texture_(_Texture)
	{
		vertex_Vector_.fAxis_X = tModel_Matrix_.Matrix()[3][0];
		vertex_Vector_.fAxis_Y = tModel_Matrix_.Matrix()[3][1];
	}

	SVertexVector& CStaticObject::GetVertexVector() { return vertex_Vector_; }
	const char& CStaticObject::GetSign() { return kSign_; }
	CTexture& CStaticObject::GetTexture() { return Texture_; }
	void CStaticObject::SetMatrix(Math::TCMatrix<MATRIX_RANGE>& _tModel_Matrix)
	{
		tModel_Matrix_ = _tModel_Matrix;
		vertex_Vector_.fAxis_X = tModel_Matrix_.Matrix()[3][0];
		vertex_Vector_.fAxis_Y = tModel_Matrix_.Matrix()[3][1];
	}
	
	Math::TCMatrix<MATRIX_RANGE>* CStaticObject::GetMatrix() { return &tModel_Matrix_; }

}
