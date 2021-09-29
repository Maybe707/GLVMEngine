#include "Collision.hpp"

namespace GLVM::Core
{
    
	void CCollision::Repel(CPlayer& _Player, double& _fDelta_Time, CEvent& _Event)
	{
		if (_Event.GetEvent() == eMOVE_UP)
		{
			_Player.GetVertexVector().fAxis_Y = (_Player.GetVertexVector().fAxis_Y - _fDelta_Time);
			_Player.GetMatrix()->Matrix()[3][1] -= _fDelta_Time;
		}
		if (_Event.GetEvent() == eMOVE_DOWN)
		{
			_Player.GetVertexVector().fAxis_Y = (_Player.GetVertexVector().fAxis_Y + _fDelta_Time);
			_Player.GetMatrix()->Matrix()[3][1] += _fDelta_Time;
		}
		if (_Event.GetEvent() == eMOVE_RIGHT)
		{
			_Player.GetVertexVector().fAxis_X = (_Player.GetVertexVector().fAxis_X - _fDelta_Time);
			_Player.GetMatrix()->Matrix()[3][0] -= _fDelta_Time;
		}
		if (_Event.GetEvent() == eMOVE_LEFT)
		{
			_Player.GetVertexVector().fAxis_X = (_Player.GetVertexVector().fAxis_X + _fDelta_Time);
			_Player.GetMatrix()->Matrix()[3][0] += _fDelta_Time;
		}
	}

	bool CCollision::BoxCollider(CPlayer& _Player, IGameObject& _Game_Object)
	{
		bool bCollision_Flag = false;
		float fX = 0;
		float fY = 0;
		std::cout << _Game_Object.GetVertexVector().fAxis_X << std::endl;
		std::cout << _Game_Object.GetVertexVector().fAxis_Y << std::endl;
		std::cout << _Player.GetVertexVector().fAxis_X << std::endl;
		std::cout << _Player.GetVertexVector().fAxis_Y << std::endl;
		if(_Game_Object.GetSign() == 's')
		{
			fX = std::abs(_Game_Object.GetVertexVector().fAxis_X - _Player.GetVertexVector().fAxis_X);
			fY = std::abs(_Game_Object.GetVertexVector().fAxis_Y - _Player.GetVertexVector().fAxis_Y);
		}
		std::cout << fX << " X" << std::endl;
		std::cout << fY << " Y" << std::endl;
		if(fX < 64.0f && fY < 64.0f)
			bCollision_Flag = true;

		//	std::cout << fAxis_X << std::endl;
//		std::cout << fAxis_Y << std::endl;
		std::cout << bCollision_Flag << std::endl;
		return bCollision_Flag;
	}

	void CCollision::Detection(TCVectorContainer<IGameObject*>& _tWorld_Container, CPlayer& _Player, double& _fDelta_Time, CEvent& _Event)
	{
		for(int i = 0; i < _tWorld_Container.GetSize(); ++i)
		{
			if(BoxCollider(_Player, *_tWorld_Container.GetVectorContainer()[i]))
				Repel(_Player, _fDelta_Time, _Event);
		}
	}

}
