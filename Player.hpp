#ifndef PLAYER
#define PLAYER

#include "Matrix.hpp"
#include "Texture.hpp"
#include "GameObject.hpp"

#define X_AXIS 12
#define Y_AXIS 13
#define MATRIX_RANGE 4

namespace GLVM::Core
{
	class CPlayer : public IGameObject
	{
		CTexture Texture_;
		Math::TCMatrix<MATRIX_RANGE> tModel_Matrix_;
	public:
		CPlayer(const CTexture& _Texture) : Texture_(_Texture) {}
		
		void SetTexture(const CTexture& _Texture)
		{
			Texture_ = _Texture;
		}

		virtual CTexture& GetTexture() override
		{
			return Texture_;
		}

		virtual Math::TCMatrix<MATRIX_RANGE>* GetMatrix() override
		{
			return &tModel_Matrix_;
		}
		
		void Move(double _dOffset, Core::CEvent _Event)
		{
			tModel_Matrix_.Move(_dOffset, _Event);
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
