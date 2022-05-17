#ifndef ENGINE
#define ENGINE

#include "GLPointer.h"
#include "GravitySystem.hpp"
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
#include "MoveComponent.hpp"
#include "TextureComponent.hpp"
#include "VertexComponent.hpp"
#include "AnimationMoveComponent.hpp"
#include "SystemManager.hpp"
#include "ViewComponent.hpp"
#include "EventComponent.hpp"
#include "GravityComponent.hpp"
#include "MoveComponent.hpp"
#include "TransformComponent.hpp"
#include "ViewComponent.hpp"

using Entity = unsigned int;

namespace GLVM::Core
{

	class CEngine
	{
		IWindow*       Window_;
		Time::IChrono* Chrono_;
        CEvent         Event_;
		Shader*        Shader_Program;
		float          fDelta_Time_;
		CStack         Input_Stack_;
        
		ECS::CRenderSystem*   Renderer_System;
		ECS::CCollisionSystem Collision_System;
		ECS::CAnimationSystem Animation_System;
		ECS::CMovementSystem* Movement_System;
        ECS::CGravitySystem*  Gravity_System_;
        
        ECS::CSystemManager*  System_Manager;
		
	public:
		CEngine();
		~CEngine();
		void GameLoop(ECS::CComponentManager& _ComponentManager);
		void LoadTextureData(GLVM::ECS::CTextureComponent& _Texture);
		void GameKill();
	};
		
}

#endif
