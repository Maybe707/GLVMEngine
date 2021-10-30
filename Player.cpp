#include "Player.hpp"

namespace GLVM::Core
{

	// CPlayer::CPlayer(const CTexture& _Texture) : Texture_(_Texture)
	// {
	// 	vertex_Vector_.fAxis_X = tModel_Matrix_.Matrix()[3][0];
	// 	vertex_Vector_.fAxis_Y = tModel_Matrix_.Matrix()[3][1];
	// }

	// void CPlayer::SetMatrix(Math::TCMatrix<MATRIX_RANGE>& _tModel_Matrix)
	// {
	// 	tModel_Matrix_ = _tModel_Matrix;
	// 	vertex_Vector_.fAxis_X = tModel_Matrix_.Matrix()[3][0];
	// 	vertex_Vector_.fAxis_Y = tModel_Matrix_.Matrix()[3][1];
	// }

	// SVertexVector& CPlayer::GetVertexVector() { return vertex_Vector_; }
	// const char& CPlayer::GetSign() { return kSign_; }
	// void CPlayer::SetTexture(const CTexture& _Texture) { Texture_ = _Texture; }
	// CTexture& CPlayer::GetTexture() { return Texture_; }
	// void CPlayer::Move(double _dOffset, Core::CEvent _Event)
	// { 
	// 	tModel_Matrix_.Move(_dOffset, _Event);
	// 	switch(_Event.GetEvent())
	// 	{
	// 	case EEvents::eMOVE_LEFT:
	// 		vertex_Vector_.fAxis_X -= _dOffset;
	// 		break;
	// 	case EEvents::eMOVE_RIGHT:
	// 		vertex_Vector_.fAxis_X += _dOffset;
	// 		break;
	// 	case EEvents::eMOVE_DOWN:
	// 		vertex_Vector_.fAxis_Y -= _dOffset;
	// 		break;
	// 	case EEvents::eMOVE_UP:
	// 		vertex_Vector_.fAxis_Y += _dOffset;
	// 		break;
	// 	}
	// }

	// void CPlayer::SetVertex(float* _Vertex)
	// {
    //     for(int i = 0; i < 30; ++i)
    //         aVertices_[i] = _Vertex[i];
	// 	pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(aVertices_), aVertices_, GL_STATIC_DRAW);
	// }
	
}
