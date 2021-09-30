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
#include "Animation.hpp"

namespace GLVM::Core
{

	class Engine
	{
		TCVectorContainer<IGameObject*> tWorldContainer;
		IWindow* Window_;
		Time::IChrono* Chrono_;
		CRenderer* Renderer_;
		CEvent Event_;
		Shader* Shader_Program;
		double dDelta_Time_;
		CStack Input_Stack_;
		CCollision Collision_;
		CAnimation Animation_;

	public:
		Engine();
		~Engine();
		void ControlInput(CStack& _Stack, bool& _bGame_Loop_Active, CEvent& _eEvent);
		void GameLoop(CPlayer& _Player);
		TCVectorContainer<IGameObject*>& GetWorldContainer();
		void GameKill();
	};
		
}

#endif
