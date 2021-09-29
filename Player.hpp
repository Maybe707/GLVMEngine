#ifndef PLAYER
#define PLAYER

#include "Matrix.hpp"
#include "Texture.hpp"
#include "GameObject.hpp"
#include "VertexVector.hpp"

#define X_AXIS 12
#define Y_AXIS 13
#define MATRIX_RANGE 4

namespace GLVM::Core
{
	class CPlayer : public IGameObject
	{
		SVertexVector vertex_Vector_;
		const char kSign_ = 'p';
		CTexture Texture_;
		Math::TCMatrix<MATRIX_RANGE> tModel_Matrix_;
	public:
		CPlayer(const CTexture& _Texture) : Texture_(_Texture)
		{
			vertex_Vector_.fAxis_X = tModel_Matrix_.Matrix()[3][0];
			vertex_Vector_.fAxis_Y = tModel_Matrix_.Matrix()[3][1];
		}

		virtual SVertexVector& GetVertexVector() override { return vertex_Vector_; }
		virtual const char& GetSign() override { return kSign_; }
		void SetTexture(const CTexture& _Texture) { Texture_ = _Texture; }
		virtual CTexture& GetTexture() override { return Texture_; }
		void SetMatrix(Math::TCMatrix<MATRIX_RANGE>& _tModel_Matrix)
	    {
			tModel_Matrix_ = _tModel_Matrix;
			vertex_Vector_.fAxis_X = tModel_Matrix_.Matrix()[3][0];
			vertex_Vector_.fAxis_Y = tModel_Matrix_.Matrix()[3][1];
		}
		virtual Math::TCMatrix<MATRIX_RANGE>* GetMatrix() override { return &tModel_Matrix_; }
		void Move(double _dOffset, Core::CEvent _Event)
		{
			tModel_Matrix_.Move(_dOffset, _Event);
			switch(_Event.GetEvent())
			{
			case EEvents::eMOVE_LEFT:
				vertex_Vector_.fAxis_X -= _dOffset;
				break;
			case EEvents::eMOVE_RIGHT:
				vertex_Vector_.fAxis_X += _dOffset;
				break;
			case EEvents::eMOVE_DOWN:
				vertex_Vector_.fAxis_Y -= _dOffset;
				break;
			case EEvents::eMOVE_UP:
				vertex_Vector_.fAxis_Y += _dOffset;
				break;
			}
			// switch(_Event.GetEvent())
			// {
			// case GLVM::Core::EEvents::eMOVE_LEFT:
			// 	tModel_Matrix_.GetMatrix()[X_AXIS] -= _dOffset;
			// 	break;
			// case GLVM::Core::EEvents::eMOVE_RIGHT:
			// 	tModel_Matrix_.GetMatrix()[X_AXIS] += _dOffset;
			// 	break;
			// case GLVM::Core::EEvents::eMOVE_DOWN:
			// 	tModel_Matrix_.GetMatrix()[Y_AXIS] -= _dOffset;
			// 	break;
			// case GLVM::Core::EEvents::eMOVE_UP:
			// 	tModel_Matrix_.GetMatrix()[Y_AXIS] += _dOffset;
			// 	break;
			// }
		}
	};
}

#endif
