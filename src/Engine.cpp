#include "Engine.hpp"
#include "ISoundEngine.hpp"
#include "GraphicAPI/Opengl.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "ShaderProgram.hpp"
#include "ISoundEngine.hpp"
#include "SoundEngineFactory.hpp"
#include "SystemManager.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/GUISystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/ProjectileSystem.hpp"
#include "Systems/RenderSystem.hpp"
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

#define DESTRUCTOR_3000 \
    std::cout << "You have been destructurized. [=]___[=]" << std::endl; \
    exit(1)

GLVM::Core::CEvent g_eEvent;
//GLVM::Core::CSoundEngine g_Sound_Engine;

namespace GLVM::Core
{
    CEngine* CEngine::pInstance_ = nullptr;
    std::mutex CEngine::Mutex_;

    void PlaybackSound(Sound::ISoundEngine* _sound_Engine)
    {
//        _sound_Engine->SetMasterVolume(10);
        while(1)
        {
            _sound_Engine->SoundStream();
        }
    }
    
    CEngine::CEngine()
	{
//		Window_             = CWindowCreator().Create();
        //   Render_System_Interface_ = new ECS::CRenderSystem();
        Render_System_Interface_ = new ECS::CRenderSystem();
		Chrono_                  = Time::CTimerCreator().Create();
        Sound_Engine_            = Sound::CSoundEngineFactory().CreateSoundEngine();

        Collision_System         = new ECS::CCollisionSystem(Input_Stack_);
        GUI_System               = new ECS::CGUISystem();
        Movement_System          = new ECS::CMovementSystem(Input_Stack_, Sound_Engine_);
        Physics_System_          = new ECS::CPhysicsSystem(Input_Stack_);
        Animation_System         = new ECS::CAnimationSystem();
        pProjectile_System_      = new ECS::CProjectileSystem(Input_Stack_);
        pCamera_System           = new ECS::CCameraSystem();
        
        fDelta_Time_             = 0.0;
        g_eEvent.SetEvent(eDEFAULT);
	}

	CEngine::~CEngine() {}
            
    CEngine* CEngine::GetInstance()
    {
        std::lock_guard<std::mutex> lock(Mutex_);
        if(pInstance_ == nullptr)
        {
            pInstance_ = new CEngine();
        }
        return pInstance_;
    }

	void CEngine::GameLoop()
	{
        ECS::CSystemManager*   pSystem_Manager     = ECS::CSystemManager::GetInstance();

		float fAnimation_Delta            = 0.0f;
		bool bGame_Loop_Active            = true;
		Animation_System->Animation_Delta = fAnimation_Delta;

		///< Call of ActivateSystem function must be in this order.

        pSystem_Manager->ActivateSystem(Movement_System);
        pSystem_Manager->ActivateSystem(pProjectile_System_);
		pSystem_Manager->ActivateSystem(Collision_System);
        pSystem_Manager->ActivateSystem(Physics_System_);
//		pSystem_Manager->ActivateSystem(Animation_System);
        pSystem_Manager->ActivateSystem(pCamera_System);
        pSystem_Manager->ActivateSystem(Render_System_Interface_);
        pSystem_Manager->ActivateSystem(GUI_System);

        std::thread sound_thread(PlaybackSound, std::ref(Sound_Engine_));
        sound_thread.detach();
        
		while(bGame_Loop_Active)
		{
			fDelta_Time_ = Chrono_->GetElapsed();
			Chrono_->Reset();
            
            ((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->Window.ClearDisplay();
            
			while(((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->Window.HandleEvent(g_eEvent))
			{
				Input_Stack_.ControlInput(g_eEvent);
                if(g_eEvent.GetEvent() == EEvents::eGAME_LOOP_KILL)
                    bGame_Loop_Active = false;
			}
			g_eEvent.SetLastEvent(Input_Stack_);

//            Input_Stack_.PrintStack();
            
            ((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->Window.CursorLock(g_eEvent.mouse_Pointer_Position_.iPosition_X,
                                g_eEvent.mouse_Pointer_Position_.iPosition_Y,
                                &g_eEvent.mouse_Pointer_Position_.iOffset_X,
                                &g_eEvent.mouse_Pointer_Position_.iOffset_Y);
            
			Movement_System->_dOffset                     = fDelta_Time_;
			Movement_System->_Anim_Event                  = g_eEvent.GetEvent();
			Collision_System->fDelta_Time_                = fDelta_Time_;
            pProjectile_System_->_dOffset                 = fDelta_Time_;
            Physics_System_->fDelta_Time_                 = fDelta_Time_;
            Physics_System_->fAcceleration_of_Gravity_   += (fDelta_Time_ / 20);
            Animation_System->eEvent_                     = Input_Stack_.Pop();
			Animation_System->Delta_Time                  = fDelta_Time_;            
            GUI_System->_Shader_Program                   = ((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->GUI_Shader_Program_;
//            pCamera_System->Shader_Program_               = ((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->_Shader_Program;
            pCamera_System->Render_System_                = Render_System_Interface_;
            
			pSystem_Manager->Update();
            ((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->Window.SwapBuffers();
            
//            g_Sound_Engine.SoundStream();
		}
	}
    
	void CEngine::GameKill()
	{
        ((RENDERER_TYPE_PTR)Render_System_Interface_->GetRenderSystemInstance())->Window.Close();
        delete Chrono_;
		Chrono_ = nullptr;
	}
}
