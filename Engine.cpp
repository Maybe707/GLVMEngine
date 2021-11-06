#include "Engine.hpp"
#include "ColliderComponent.hpp"
#include "ConstVectorContainer.hpp"
#include "MoveComponent.hpp"
#include "TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "VertexComponent.hpp"
#include <iostream>

#define NUMBER_OF_CREATING_TEXTURE_OBJECT_1 1
#define SOME_STRANGE_STUFF 0
#define MIPMAP_LEVEL 0
#define SOME_OLD_STUFF 0

namespace GLVM::Core
{    

	void CEngine::ControlInput(CStack& _Stack, bool& _bGame_Loop_Active, CEvent& _eEvent)
 	{ 
		switch(_eEvent.GetEvent())
		{
		case eGAME_LOOP_KILL:
			_Stack.Push(eGAME_LOOP_KILL);
			break;
		case eKEYRELEASE_A:
			_Stack.Remove(eMOVE_LEFT);
			break;
		case eKEYRELEASE_D:
			_Stack.Remove(eMOVE_RIGHT);
			break;
		case eKEYRELEASE_S:
			_Stack.Remove(eMOVE_DOWN); 
			break;
		case eKEYRELEASE_W:
			_Stack.Remove(eMOVE_UP); 
			break;
		case eMOVE_LEFT:
			_Stack.Push(eMOVE_LEFT);
			break;
		case eMOVE_RIGHT:
			_Stack.Push(eMOVE_RIGHT);
			break;
		case eMOVE_DOWN:
			_Stack.Push(eMOVE_DOWN);
			break;
		case eMOVE_UP:
			_Stack.Push(eMOVE_UP);
			break;
		default:
			break;
		}
	}

    CEngine::CEngine()
	{
		Window_ = CWindowCreator().Create();
		Chrono_ = Time::CTimerCreator().Create();
		Renderer_ = new CRenderSystem();
		Movement_ = new ECS::CMovementSystem();
		Shader_Program = new Shader();
		Event_.SetEvent(eDEFAULT);
		
		dDelta_Time_ = 0.0;
	}

	CEngine::~CEngine()
	{
		delete Renderer_;
		Renderer_ = nullptr;
		delete Shader_Program;
		Shader_Program = nullptr;
		delete Movement_;
		Movement_ = nullptr;
	}

	void CEngine::GameLoop(ECS::CComponentManager& _ComponentManager)
	{
		double dAnimation_Delta = 0;
		bool bGame_Loop_Active = true;
		while(bGame_Loop_Active)
		{
			dDelta_Time_ = Chrono_->GetElapsed();
			dDelta_Time_ *= 200;
			Chrono_->Reset();

			Window_->ClearDisplay();
			Shader_Program->Use();
			Shader_Program->SetUniformID();
			Renderer_->SetProjectionMatrix(Shader_Program);
			
			while((Window_->HandleEvent(Event_)))
			{
				Input_Stack_.ControlInput(Event_);
			}
			Event_.SetLastEvent(Input_Stack_);
			if(Event_.GetEvent() == EEvents::eGAME_LOOP_KILL)
				bGame_Loop_Active = false;
//			_Player.Move(dDelta_Time_, Event_);

//			Animation_.Walk(Input_Stack_, dAnimation_Delta, dDelta_Time_, _Player);
//			Renderer_->SetModelMatrix(Shader_Program, _Player.GetMatrix()->GetMatrix());
//			Renderer_->Draw(_Player);
//			pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(aVertices_Static_Object), aVertices_Static_Object, GL_DYNAMIC_DRAW);
			Movement_->Move(static_cast<TCConstVectorContainer<ECS::STransformComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::STransformComponent>()]), static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::SMoveComponent>()]), static_cast<TCConstVectorContainer<ECS::SMoveComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::SMoveComponent>()]), dDelta_Time_, Event_.GetEvent());
			Collision_.Detection(static_cast<TCConstVectorContainer<ECS::STransformComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::STransformComponent>()]), static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::CColliderComponent>()]), static_cast<TCConstVectorContainer<ECS::SMoveComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::SMoveComponent>()]), static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::SMoveComponent>()]), dDelta_Time_);
			Animation_.Walk(static_cast<TCConstVectorContainer<ECS::SVertexComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::SVertexComponent>()]),
							static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::SAnimationMoveComponent>()]),
							Input_Stack_, dAnimation_Delta, dDelta_Time_);
			Renderer_->DrawAll(static_cast<TCConstVectorContainer<ECS::STransformComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::STransformComponent>()]),
							   static_cast<TCConstVectorContainer<ECS::CTextureComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::CTextureComponent>()]),
							   static_cast<TCConstVectorContainer<ECS::SVertexComponent>*>(_ComponentManager.tMain_Container_[_ComponentManager.CreateComponentContainer<ECS::SVertexComponent>()]),
							   static_cast<TCVectorContainer<unsigned int>*>(_ComponentManager.tOrdered_Container_[_ComponentManager.CreateComponentContainer<ECS::SVertexComponent>()]),
							   Shader_Program);
			Window_->SwapBuffers();
		}
	}
	
//	TCVectorContainer<IGameObject*>& CEngine::GetWorldContainer() { return tWorldContainer; }

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

