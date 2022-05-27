#include "Engine.hpp"
#include "ShaderProgram.hpp"
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

namespace GLVM::Core
{
    CEngine* CEngine::pInstance_ = nullptr;
    std::mutex CEngine::Mutex_;
    
    CEngine::CEngine()
	{
		Window_             = CWindowCreator().Create();
		Chrono_             = Time::CTimerCreator().Create();

        Collision_System    = new ECS::CCollisionSystem(Input_Stack_);
        GUI_System          = new ECS::CGUISystem();
        Renderer_System     = new ECS::CRenderSystem();
        Movement_System     = new ECS::CMovementSystem(Input_Stack_);
        Physics_System_     = new ECS::CPhysicsSystem(Input_Stack_);
        Animation_System    = new ECS::CAnimationSystem();
        pProjectile_System_ = new ECS::CProjectileSystem();
        pCamera_System      = new ECS::CCameraSystem();
        
		Shader_Program      = new Shader("../Shader.vs", "../Shader.fs");
        GUI_Shader_Program_ = new Shader("../GUIShader.vs", "../GUIShader.fs");
        
        fDelta_Time_        = 0.0;
        
        g_eEvent.SetEvent(eDEFAULT);
	}

	CEngine::~CEngine()
	{
		// delete Renderer_System;
		// delete Shader_Program;
		// delete Movement_System;
        // delete Physics_System_;
        // delete GUI_System;
        // delete Animation_System;
        // Renderer_System  = nullptr;
        // Shader_Program   = nullptr;
        // Physics_System_  = nullptr;
        // Movement_System  = nullptr;
        // GUI_System       = nullptr;
        // Animation_System = nullptr;
	}

    CEngine* CEngine::GetInstance()
    {
        std::lock_guard<std::mutex> lock(Mutex_);
        if(pInstance_ == nullptr)
        {
            pInstance_ = new CEngine();
        }
        return pInstance_;
    }
    
	void CEngine::GameLoop(ECS::CComponentManager& _ComponentManager)
	{
        ECS::CSystemManager*   pSystem_Manager     = ECS::CSystemManager::GetInstance();

        // ECS::CCollisionSystem* Collision_System    = new ECS::CCollisionSystem(Input_Stack_);
        // ECS::CGUISystem*       GUI_System          = new ECS::CGUISystem();
        // ECS::CRenderSystem*    Renderer_System     = new ECS::CRenderSystem();
        // ECS::CMovementSystem*  Movement_System     = new ECS::CMovementSystem(Input_Stack_);
        // ECS::CPhysicsSystem*   Physics_System_     = new ECS::CPhysicsSystem(Input_Stack_);
        // ECS::CAnimationSystem* Animation_System    = new ECS::CAnimationSystem();

		float fAnimation_Delta            = 0.0f;
		bool bGame_Loop_Active            = true;
		Animation_System->Animation_Delta = fAnimation_Delta;

		///< Call of ActivateSystem function must be in this order. 

        pSystem_Manager->ActivateSystem(Movement_System);
		pSystem_Manager->ActivateSystem(Collision_System);
//        pSystem_Manager->ActivateSystem(pProjectile_System_);
        pSystem_Manager->ActivateSystem(Physics_System_);
		pSystem_Manager->ActivateSystem(Animation_System);
        pSystem_Manager->ActivateSystem(pCamera_System);
        pSystem_Manager->ActivateSystem(Renderer_System);
        pSystem_Manager->ActivateSystem(GUI_System);

		while(bGame_Loop_Active)
		{
			fDelta_Time_ = Chrono_->GetElapsed();
			Chrono_->Reset();
            
			Window_->ClearDisplay();

            Shader_Program->Use();
			Shader_Program->SetUniformID();

			while((Window_->HandleEvent(g_eEvent)))
			{
				Input_Stack_.ControlInput(g_eEvent);
                if(g_eEvent.GetEvent() == EEvents::eGAME_LOOP_KILL)
                    bGame_Loop_Active = false;
			}
			g_eEvent.SetLastEvent(Input_Stack_);

//            Input_Stack_.PrintStack();
            
            Window_->CursorLock(g_eEvent.mouse_Pointer_Position_.iPosition_X,
                                g_eEvent.mouse_Pointer_Position_.iPosition_Y,
                                &g_eEvent.mouse_Pointer_Position_.iOffset_X,
                                &g_eEvent.mouse_Pointer_Position_.iOffset_Y);
            
			Movement_System->_dOffset                     = fDelta_Time_;
			Movement_System->_Anim_Event                  = g_eEvent.GetEvent();
			Collision_System->fDelta_Time_                = fDelta_Time_;
            Physics_System_->fDelta_Time_                 = fDelta_Time_;
//            std::cout << "Delta: " << fDelta_Time_ << std::endl;
            Physics_System_->fAcceleration_of_Gravity_   += (fDelta_Time_ / 20);
			Animation_System->eEvent_                     = Input_Stack_.Pop();
			Animation_System->Delta_Time                  = fDelta_Time_;
			Renderer_System->_Shader_Program              = Shader_Program;
            GUI_System->_Shader_Program                   = GUI_Shader_Program_;
            pCamera_System->Shader_Program_               = Shader_Program;
            
			pSystem_Manager->Update();
            Window_->SwapBuffers();
		}

        // delete pSystem_Manager;
        // delete Renderer_System;
		// delete Shader_Program;
		// delete Movement_System;
        // delete Physics_System_;
        // delete GUI_System;
        // delete Animation_System;
        // pSystem_Manager  = nullptr;
        // Renderer_System  = nullptr;
        // Shader_Program   = nullptr;
        // Physics_System_  = nullptr;
        // Movement_System  = nullptr;
        // GUI_System       = nullptr;
        // Animation_System = nullptr;
	}

 	void CEngine::LoadTextureData(GLVM::ECS::CTextureComponent& _Texture)
	{
		///< Loading and creating texture.
		glGenTextures(NUMBER_OF_CREATING_TEXTURE_OBJECT_1, &_Texture.iTexture_);
		glBindTexture(GL_TEXTURE_2D, _Texture.iTexture_);
	
		///< Setting texture applying parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		///< Loading image, creating texture and generation mipmap-levels
		glTexImage2D(GL_TEXTURE_2D, MIPMAP_LEVEL, GL_RGBA, _Texture.iWidth_, _Texture.iHeight_, SOME_OLD_STUFF, GL_RGBA, GL_UNSIGNED_BYTE, _Texture.u_iData_);
		pGLGenerate_Mipmap(GL_TEXTURE_2D);

		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	void CEngine::GameKill()
	{
	   	Window_->Close();
		delete Window_;
        delete Chrono_;
		Window_ = nullptr;
		Chrono_ = nullptr;


	}
}

