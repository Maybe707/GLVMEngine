#ifndef ENGINE
#define ENGINE

#include "GLPointer.h"
#include "Systems/GravitySystem.hpp"
#include "IChrono.hpp"
#include "IWindow.hpp"
#include "Systems/RenderSystem.hpp"
#include "ShaderProgram.hpp"
#include "Stack.hpp"
#include "Event.hpp"
#include "VertexData.hpp"
#include "WindowCreator.hpp"
#include "TimerCreator.hpp"
#include "Components/TextureComponent.hpp"
#include "VectorContainer.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "ComponentManager.hpp"
#include "GLPointer.h"
#include "IWindow.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <GL/glext.h>
#include "VertexData.hpp"
#include "Systems/MovementSystem.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "SystemManager.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/EventComponent.hpp"
#include "Components/GravityComponent.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"

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
		ECS::CCollisionSystem* Collision_System;
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
