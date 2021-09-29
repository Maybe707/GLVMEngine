#include "Engine.hpp"
#include "IWindow.hpp"

namespace GLVM::Core
{    

	void Engine::ControlInput(CStack& _Stack, bool& _bGame_Loop_Active, CEvent& _eEvent)
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
		}
	}

    Engine::Engine()
	{
		Window_ = CWindowCreator().Create();
		Chrono_ = Time::CTimerCreator().Create();
		Renderer_ = new CRenderer();
		Shader_Program = new Shader();
		Event_.SetEvent(eDEFAULT);

		
		dDelta_Time_ = 0.0;
		bGame_Loop_Active_ = true;
	}

	Engine::~Engine()
	{
		delete Renderer_;
		Renderer_ = nullptr;
		delete Shader_Program;
		Shader_Program = nullptr;
	}

	void Engine::GameLoop(CPlayer& _Player)
	{
		while(bGame_Loop_Active_)
		{
			dDelta_Time_ = Chrono_->GetElapsed();
			dDelta_Time_ *= 200;
			Chrono_->Reset();

			Window_->ClearDisplay();
			Shader_Program->Use();
			Shader_Program->SetUniformID();
			Renderer_->SetProjectionMatrix(Shader_Program);
//			Renderer_->SetModelMatrix(Shader_Program, _tMatrix2.GetMatrix());
//			Renderer_->Draw(_Texture2);


			
			while((Window_->HandleEvent(Event_)))
			{
				Input_Stack_.ControlInput(Event_);
			}
			Event_.SetLastEvent(Input_Stack_);
			if(Event_.GetEvent() == EEvents::eGAME_LOOP_KILL)
				bGame_Loop_Active_ = false;
			_Player.Move(dDelta_Time_, Event_);
			Collision_.Detection(tWorldContainer, _Player, dDelta_Time_, Event_);
//			Input_Stack_.Show();
			Renderer_->SetModelMatrix(Shader_Program, _Player.GetMatrix()->GetMatrix());
			Renderer_->Draw(_Player);
			Renderer_->DrawAll(&tWorldContainer, Shader_Program);
			Window_->SwapBuffers();
		}
	}

	void Engine::GameKill()
	{
	   	Window_->Close();
		delete Window_;
		Window_ = nullptr;
		delete Chrono_;
		Chrono_ = nullptr;
	}
}

