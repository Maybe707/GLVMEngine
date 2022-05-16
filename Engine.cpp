#include "Engine.hpp"
#include "AnimationMoveComponent.hpp"
#include "ColliderComponent.hpp"
#include "ComponentManager.hpp"
#include "GravitySystem.hpp"
#include "MoveComponent.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "VertexComponent.hpp"
#include "IContainer.hpp"
#include "ViewComponent.hpp"

#define NUMBER_OF_CREATING_TEXTURE_OBJECT_1 1
#define SOME_STRANGE_STUFF                  0
#define MIPMAP_LEVEL                        0
#define SOME_OLD_STUFF 0

/*******************************************************************
 * Legends never die...
 * You are about to face most terrifying data structures of all time.
 *    "Abandon hope all ye who enter here..." (c) Dante Alighieri.
 *******************************************************************
 *****************  👑  !!!  DESTRUCTOR_3000  !!!  👑  *************/

#define DESTRUCTOR_3000 \
    std::cout << "You have been destructurized. [=]___[=]" << std::endl; \
    exit(1)

namespace GLVM::Core
{    
    CEngine::CEngine()
	{
		Window_         = CWindowCreator().Create();
		Chrono_         = Time::CTimerCreator().Create();
        
		Renderer_System = new ECS::CRenderSystem();
		Movement_System = new ECS::CMovementSystem(Input_Stack_);
        Gravity_System_ = new ECS::CGravitySystem();
        
		Shader_Program  = new Shader();
        System_Manager  = new ECS::CSystemManager();
        
        fDelta_Time_    = 0.0;
        
        Event_.SetEvent(eDEFAULT);
	}

	CEngine::~CEngine()
	{
		delete Renderer_System;
		delete Shader_Program;
		delete Movement_System;
        delete Gravity_System_;
		delete System_Manager;
		System_Manager  = nullptr;
        Renderer_System = nullptr;
        Shader_Program  = nullptr;
        Gravity_System_ = nullptr;
        Movement_System = nullptr;
	}
	
	void CEngine::GameLoop(ECS::CComponentManager& _ComponentManager)
	{
		float fAnimation_Delta           = 0.0f;
		bool bGame_Loop_Active           = true;
		Animation_System.Animation_Delta = fAnimation_Delta;

		///< Call of ActivateSystem function must be in this order. 
                 
        System_Manager->ActivateSystem(Gravity_System_);
        System_Manager->ActivateSystem(Movement_System);
		System_Manager->ActivateSystem(&Collision_System);
		System_Manager->ActivateSystem(&Animation_System);
        System_Manager->ActivateSystem(Renderer_System);

		while(bGame_Loop_Active)
		{
			fDelta_Time_ = Chrono_->GetElapsed();
			Chrono_->Reset();
            
			Window_->ClearDisplay();

            Shader_Program->Use();
			Shader_Program->SetUniformID();
			Renderer_System->SetProjectionMatrix(Shader_Program);
			while((Window_->HandleEvent(Event_)))
			{
				Input_Stack_.ControlInput(Event_);
                if(Event_.GetEvent() == EEvents::eGAME_LOOP_KILL)
                    bGame_Loop_Active = false;
			}
			Event_.SetLastEvent(Input_Stack_);
            
            Window_->CursorLock(Event_.mouse_Pointer_Position_.iPosition_X,
                                Event_.mouse_Pointer_Position_.iPosition_Y,
                                &Event_.mouse_Pointer_Position_.iOffset_X,
                                &Event_.mouse_Pointer_Position_.iOffset_Y);

            Gravity_System_->fAcceleration_of_Gravity_   += (fDelta_Time_ / 20);
			Movement_System->_dOffset                     = fDelta_Time_;
			Movement_System->_Anim_Event                  = Event_.GetEvent();
			Collision_System.fDelta_Time_                 = fDelta_Time_;
			Animation_System.eEvent_                      = Input_Stack_.Pop();
			Animation_System.Delta_Time                   = fDelta_Time_;
			Renderer_System->_Shader_Program              = Shader_Program;
			
			System_Manager->Update(_ComponentManager, Event_);
			
			Window_->SwapBuffers();
		}
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

