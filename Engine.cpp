#include "Engine.hpp"
#include "ColliderComponent.hpp"
#include "ConstVectorContainer.hpp"
#include "MoveComponent.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "VertexComponent.hpp"
#include <iostream>
#include <typeinfo>
#include "IContainer.hpp"

#define NUMBER_OF_CREATING_TEXTURE_OBJECT_1 1
#define SOME_STRANGE_STUFF 0
#define MIPMAP_LEVEL 0
#define SOME_OLD_STUFF 0

namespace GLVM::Core
{    
    CEngine::CEngine()
	{
		Window_ = CWindowCreator().Create();
		Chrono_ = Time::CTimerCreator().Create();
		Renderer_System = new ECS::CRenderSystem();
		Movement_System = new ECS::CMovementSystem();
		Shader_Program = new Shader();
		Event_.SetEvent(eDEFAULT);
		
		dDelta_Time_ = 0.0;
	}

	CEngine::~CEngine()
	{
		delete Renderer_System;
		Renderer_System = nullptr;
		delete Shader_Program;
		Shader_Program = nullptr;
		delete Movement_System;
		Movement_System = nullptr;
	}

	void CEngine::GameLoop(ECS::CComponentManager& _ComponentManager)
	{
		double dAnimation_Delta = 0;
		bool bGame_Loop_Active = true;
		
		TCConstVectorContainer<ECS::STransformComponent>* pIner_Object_Component_Container_Of_Type_STransformComponent = static_cast<TCConstVectorContainer<ECS::STransformComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::STransformComponent>()]);
		TCVectorContainer<unsigned int>* pIner_Index_Component_Container_Of_Type_SMoveComponent = static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::SMoveComponent>()]);
		TCConstVectorContainer<ECS::SMoveComponent>* pIner_Object_Component_Container_Of_Type_SMoveComponent = static_cast<TCConstVectorContainer<ECS::SMoveComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::SMoveComponent>()]);
		TCVectorContainer<unsigned int>* pIner_Index_ComponentContainer_Of_Type_CColliderComponent = static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::CColliderComponent>()]);
		TCConstVectorContainer<ECS::SVertexComponent>* pIner_Object_Component_Container_Of_Type_SVertexComponent = static_cast<TCConstVectorContainer<ECS::SVertexComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::SVertexComponent>()]);
		TCVectorContainer<unsigned int>* pIner_Index_Component_Container_Of_Type_SAnimationMoveComponent = static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::SAnimationMoveComponent>()]);
		TCConstVectorContainer<ECS::CTextureComponent>* pIner_Object_Component_Container_Of_Type_CTextureComponent = static_cast<TCConstVectorContainer<ECS::CTextureComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::CTextureComponent>()]);
		TCVectorContainer<unsigned int>* pIner_Index_Component_Container_Of_Type_SVertexComponent = static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::SVertexComponent>()]);
		TCVectorContainer<unsigned int>* pIner_Index_Component_Container_Of_Type_CTextureComponent = static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::CTextureComponent>()]);
		
		while(bGame_Loop_Active)
		{
			dDelta_Time_ = Chrono_->GetElapsed();
			dDelta_Time_ *= 200;
			Chrono_->Reset();

			Window_->ClearDisplay();
			Shader_Program->Use();
			Shader_Program->SetUniformID();
			Renderer_System->SetProjectionMatrix(Shader_Program);
			
			while((Window_->HandleEvent(Event_)))
			{
				Input_Stack_.ControlInput(Event_);
			}
			Event_.SetLastEvent(Input_Stack_);
			if(Event_.GetEvent() == EEvents::eGAME_LOOP_KILL)
				bGame_Loop_Active = false;

			Movement_System->Move
			(
				pIner_Object_Component_Container_Of_Type_STransformComponent,
				pIner_Index_Component_Container_Of_Type_SMoveComponent,
				pIner_Object_Component_Container_Of_Type_SMoveComponent,
				dDelta_Time_, Event_.GetEvent()
			);

			Collision_System.Detection
			(
				pIner_Object_Component_Container_Of_Type_STransformComponent,
				pIner_Index_ComponentContainer_Of_Type_CColliderComponent,
				pIner_Object_Component_Container_Of_Type_SMoveComponent,
				pIner_Index_Component_Container_Of_Type_SMoveComponent,
				dDelta_Time_
			);

			Animation_System.Walk
			(
				pIner_Object_Component_Container_Of_Type_SVertexComponent,
				pIner_Index_Component_Container_Of_Type_SAnimationMoveComponent,
				Input_Stack_, dAnimation_Delta, dDelta_Time_
			);
			
			Renderer_System->DrawAll
			(
				pIner_Object_Component_Container_Of_Type_STransformComponent,
				pIner_Object_Component_Container_Of_Type_CTextureComponent,
				pIner_Object_Component_Container_Of_Type_SVertexComponent,
				pIner_Index_Component_Container_Of_Type_SVertexComponent,
				pIner_Index_Component_Container_Of_Type_CTextureComponent,
				Shader_Program
			);
			
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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	void CEngine::GameKill()
	{
	   	Window_->Close();
		delete Window_;
		Window_ = nullptr;
		delete Chrono_;
		Chrono_ = nullptr;
	}
}

