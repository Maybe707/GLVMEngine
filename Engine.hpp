#ifndef ENGINE
#define ENGINE

#include "GLPointer.h"
#include "IChrono.hpp"
#include "IWindow.hpp"
#include "Renderer.hpp"
#include "ShaderProgram.hpp"
#include "Stack.hpp"
#include "Event.hpp"
#include "VertexData.hpp"
#include "WindowCreator.hpp"
#include "TimerCreator.hpp"
#include "Player.hpp"
#include "Texture.hpp"
#include "Matrix.hpp"
#include "GameObject.hpp"
#include "StaticObject.hpp"
#include "VectorContainer.hpp"
#include "Collision.hpp"

namespace GLVM::Core
{

	class Engine
	{
	public:
		TCVectorContainer<IGameObject*> tWorldContainer;
		IWindow* Window_;
		Time::IChrono* Chrono_;
		CRenderer* Renderer_;
		CEvent Event_;
		Shader* Shader_Program;
		double dDelta_Time_;
		bool bGame_Loop_Active_;
		CStack Input_Stack_;
		CCollision Collision_;
		
		Engine();
		~Engine();
		void ControlInput(CStack& _Stack, bool& _bGame_Loop_Active, CEvent& _eEvent);
		void GameLoop(CPlayer& _Player);
		void GameKill();
	};
		
}

#endif
