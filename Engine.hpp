#ifndef ENGINE
#define ENGINE

#include "GLPointer.h"
#include "IChrono.hpp"
#include "IWindow.hpp"
#include "RenderSystem.hpp"
#include "ShaderProgram.hpp"
#include "Stack.hpp"
#include "Event.hpp"
#include "VertexData.hpp"
#include "WindowCreator.hpp"
#include "TimerCreator.hpp"
#include "TextureComponent.hpp"
#include "MatrixComponent.hpp"
#include "VectorContainer.hpp"
#include "CollisionSystem.hpp"
#include "AnimationSystem.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "ColliderComponent.hpp"
#include "ComponentManager.hpp"
#include "GLPointer.h"
#include "IWindow.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include <GL/glext.h>
#include "VertexData.hpp"
#include "MovementSystem.hpp"
#include "ConstVectorContainer.hpp"
#include "MoveComponent.hpp"
#include "TextureComponent.hpp"
#include "VertexComponent.hpp"

using Entity = unsigned int;

namespace GLVM::Core
{

	class CEngine
	{
		IWindow* Window_;
		Time::IChrono* Chrono_;
		CRenderSystem* Renderer_;
		CEvent Event_;
		Shader* Shader_Program;
		double dDelta_Time_;
		CStack Input_Stack_;
		ECS::CCollisionSystem Collision_;
		ECS::CAnimation Animation_;
		ECS::CMovementSystem* Movement_;
		
	public:
		CEngine();
		~CEngine();
		void ControlInput(CStack& _Stack, bool& _bGame_Loop_Active, CEvent& _eEvent);
		void GameLoop(ECS::CComponentManager& _ComponentManager);
		void LoadTextureData(GLVM::ECS::CTextureComponent& _Texture);
		void GameKill();
	};
		
}

#endif
