#ifndef ENGINE
#define ENGINE

#include "GLPointer.h"
#include "Systems/GUISystem.hpp"
#include "IChrono.hpp"
#include "IWindow.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/ProjectileSystem.hpp"
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
#include "IWindow.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "VertexData.hpp"
#include "Systems/MovementSystem.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "SystemManager.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/EventComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "ComponentManager.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Event.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/GUISystem.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/TextureComponent.hpp"
#include "VectorContainer.hpp"
#include "Components/VertexComponent.hpp"
#include "IContainer.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include "Constants.hpp"
#include <mutex>

using Entity = unsigned int;

namespace GLVM::Core
{
	class CEngine
	{
        static CEngine* pInstance_;
        static std::mutex  Mutex_;
        
		IWindow*       Window_;
		Time::IChrono* Chrono_;
		Shader*        Shader_Program;
        Shader*        GUI_Shader_Program_;
		float          fDelta_Time_;
		CStack         Input_Stack_;
        
		ECS::CRenderSystem*     Renderer_System;
        ECS::CCollisionSystem*  Collision_System;
		ECS::CAnimationSystem*  Animation_System;
		ECS::CMovementSystem*   Movement_System;
        ECS::CPhysicsSystem*    Physics_System_;
        ECS::CProjectileSystem* pProjectile_System_;

        CEngine();
        
	public:
        ~CEngine();
        
        CEngine(CEngine& _engine) = delete;                   ///< Dont need to make cope because of singleton property.
        void operator=(const CEngine& _engine) = delete;      ///< Dont need assignment operator because of singleton property.
        static CEngine* GetInstance();                        ///< It possibly to get only one instance of this class whith this method.
        
		void GameLoop(ECS::CComponentManager& _ComponentManager);
		void LoadTextureData(GLVM::ECS::CTextureComponent& _Texture);
		void GameKill();

        ECS::CGUISystem*       GUI_System;
	};
		
}

#endif
