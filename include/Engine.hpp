#ifndef ENGINE
#define ENGINE

#include "ComponentsFullSet.hpp"
#include "SystemsFullSet.hpp"
#include "GLPointer.h"
#include "IChrono.hpp"
#include "IWindow.hpp"
#include "ISoundEngine.hpp"
#include "ShaderProgram.hpp"
#include "Stack.hpp"
#include "Event.hpp"
#include "WindowCreator.hpp"
#include "TimerCreator.hpp"
#include "Vector.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"
#include "IContainer.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include "Constants.hpp"
#include <mutex>
#include "TextureManager.hpp"

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
