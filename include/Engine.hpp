#ifndef ENGINE
#define ENGINE

#include "Components/DirectionalLightComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "GLPointer.h"
#include "Systems/GUISystem.hpp"
#include "IChrono.hpp"
#include "IWindow.hpp"
#include "ISoundEngine.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/ProjectileSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "ShaderProgram.hpp"
#include "Stack.hpp"
#include "Event.hpp"
#include "VertexData.hpp"
#include "WindowCreator.hpp"
#include "TimerCreator.hpp"
#include "Components/MaterialComponent.hpp"
#include "Vector.hpp"
#include "Systems/CollisionSystem.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "IWindow.hpp"
#include "Components/MaterialComponent.hpp"
#include "VertexData.hpp"
#include "Systems/MovementSystem.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/MaterialComponent.hpp"
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
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Vector.hpp"
#include "Components/VertexComponent.hpp"
#include "IContainer.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include "Constants.hpp"
#include <mutex>
#include "Systems/RenderSystem.hpp"
#include "TextureManager.hpp"
#include "Components/ControllerComponent.hpp"

using Entity = unsigned int;

namespace GLVM::core
{
	class Engine
	{
        static Engine*    pInstance_;
        static std::mutex  Mutex_;
        
		Time::IChrono       * chrono;
        Sound::ISoundEngine * soundEngine;

		float              deltaFrameTime;
		float              gravity;
		CStack             Input_Stack_;
        ecs::CRenderSystem * renderSystemInterface;
        
        ecs::CCollisionSystem  * collisionSystem;
		ecs::CMovementSystem   * movementSystem;
        ecs::CPhysicsSystem    * physicsSystem;
        ecs::CProjectileSystem * projectileSystem;
		ecs::CCameraSystem     * cameraSystem;
//        ecs::CGUISystem        * GUI_System;

		/// For FPS counting
		unsigned int fpsCounter = 0;
		double fpsAccumulator   = 0;
		
        Engine();
        
	public:

        
        ~Engine();
        
        Engine(Engine& _engine) = delete;                   ///< Dont need to make copy because of singleton property.
        void operator=(const Engine& _engine) = delete;      ///< Dont need assignment operator because of singleton property.
        static Engine* GetInstance();                        ///< It possibly to get only one instance of this class whith this method
        
		void GameLoop();
		void FPScounter();
		void GameKill();
//        void PlaybackSound(core::CSoundEngine& _sound_Engine);
	};
		
}

#endif
