#include "Engine.hpp"
#include "ISoundEngine.hpp"
#include "GraphicAPI/Opengl.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "ShaderProgram.hpp"
#include "ISoundEngine.hpp"
#include "SoundEngineFactory.hpp"
#include "SystemManager.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/GUISystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/ProjectileSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include <limits>
#include <mutex>
#include <thread>

#ifdef OPENGL_API
#define RENDERER_TYPE_PTR COpenglRenderer*
#endif

#ifdef VULKAN_API
#define RENDERER_TYPE_PTR CVulkanRenderer*
#endif

/*******************************************************************
 * Legends never die...
 * You are about to face most terrifying data structures of all time.
 *    "Abandon hope all ye who enter here..." (c) Dante Alighieri.
 *******************************************************************
 *****************  👑  !!!  DESTRUCTOR_3000  !!!  👑  *************/

#define DESTRUCTOR_3000													\
    std::cout << "You have been destructurized. [=]___[=]" << std::endl; \
    exit(1)

GLVM::core::CEvent g_eEvent;
//GLVM::core::CSoundEngine g_Sound_Engine;

namespace GLVM::core
{
    Engine* Engine::pInstance_ = nullptr;
    std::mutex Engine::Mutex_;

    void PlaybackSound(Sound::ISoundEngine* _sound_Engine)
    {
		//        _sound_Engine->SetMasterVolume(10);
        while(1)
		{
			_sound_Engine->SoundStream();
		}
    }
    
    Engine::Engine()
    {
		//		Window_             = CWindowCreator().Create();
		//   Render_System_Interface_ = new ecs::CRenderSystem();
		renderSystemInterface    = new ecs::CRenderSystem();
		chrono                   = Time::CTimerCreator().Create();
		soundEngine              = Sound::CSoundEngineFactory().CreateSoundEngine();

		collisionSystem          = new ecs::CCollisionSystem(Input_Stack_);
		GUI_System               = new ecs::CGUISystem();
		movementSystem           = new ecs::CMovementSystem(Input_Stack_);
		physicsSystem            = new ecs::CPhysicsSystem(Input_Stack_);
		projectileSystem         = new ecs::CProjectileSystem(Input_Stack_);
//		pCamera_System           = new ecs::CCameraSystem();
        
		deltaFrameTime             = 0.0;
		g_eEvent.SetEvent(eDEFAULT);
    }

    Engine::~Engine() {}
            
    Engine* Engine::GetInstance()
    {
		std::lock_guard<std::mutex> lock(Mutex_);
		if(pInstance_ == nullptr)
		{
			pInstance_ = new Engine();
		}
		return pInstance_;
    }

    void Engine::GameLoop()
    {
		ecs::CSystemManager* pSystem_Manager = ecs::CSystemManager::GetInstance();
		bool bGame_Loop_Active = true;

		///< Call of ActivateSystem function must be in this order.

		pSystem_Manager->ActivateSystem(movementSystem);
		pSystem_Manager->ActivateSystem(collisionSystem);
		pSystem_Manager->ActivateSystem(projectileSystem);
		pSystem_Manager->ActivateSystem(physicsSystem);
		//		pSystem_Manager->ActivateSystem(Animation_System);
//		pSystem_Manager->ActivateSystem(pCamera_System);
		pSystem_Manager->ActivateSystem(renderSystemInterface);
		pSystem_Manager->ActivateSystem(GUI_System);

		std::thread sound_thread(PlaybackSound, std::ref(soundEngine));
		sound_thread.detach();

		
		while(bGame_Loop_Active)
		{
			deltaFrameTime = chrono->GetElapsed();
			chrono->Reset();

//			FPScounter();
			
			((RENDERER_TYPE_PTR)renderSystemInterface->GetRenderSystemInstance())->Window.ClearDisplay();
             
			while(((RENDERER_TYPE_PTR)renderSystemInterface->GetRenderSystemInstance())->Window.HandleEvent(g_eEvent))
			{
				//                std::cout << g_eEvent.GetEvent() << std::endl;
				Input_Stack_.ControlInput(g_eEvent);
				if(g_eEvent.GetEvent() == EEvents::eGAME_LOOP_KILL)
					bGame_Loop_Active = false;
			}
			g_eEvent.SetLastEvent(Input_Stack_);

			//            Input_Stack_.PrintStack();
            
			((RENDERER_TYPE_PTR)renderSystemInterface->GetRenderSystemInstance())->
				Window.CursorLock(g_eEvent.mousePointerPosition.position_X,
								  g_eEvent.mousePointerPosition.position_Y,
								  &g_eEvent.mousePointerPosition.offset_X,
								  &g_eEvent.mousePointerPosition.offset_Y);

			movementSystem->deltaFrameTime               = deltaFrameTime;
			collisionSystem->fDelta_Time_                = deltaFrameTime;
			projectileSystem->deltaFrameTime           = deltaFrameTime;
			physicsSystem->fDelta_Time_                 = deltaFrameTime;
			physicsSystem->fAcceleration_of_Gravity_   += (deltaFrameTime / 20);
			//            GUI_System->_Shader_Program                   = ((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->GUI_Shader_Program_;
			//            pCamera_System->Shader_Program_               = ((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->_Shader_Program;
//			pCamera_System->Render_System_                = Render_System_Interface_;
			std::cout << "Frame" << std::endl;
			pSystem_Manager->Update();
			((RENDERER_TYPE_PTR)renderSystemInterface->GetRenderSystemInstance())->Window.SwapBuffers();
			//            g_Sound_Engine.SoundStream();
		}
    }

	void Engine::FPScounter() {
		++fpsCounter;
		fpsAccumulator += deltaFrameTime;
		if (fpsAccumulator > 1.0f) {
			std::cout << "FPS: " << fpsCounter << std::endl;
			fpsCounter = 0;
			fpsAccumulator = 0;
		}
	}
	
    void Engine::GameKill()
    {
		((RENDERER_TYPE_PTR)renderSystemInterface->GetRenderSystemInstance())->Window.Close();
		delete chrono;
		chrono = nullptr;
    }
}
