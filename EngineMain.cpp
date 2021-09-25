#include "GLPointer.h"
#include "IWindow.hpp"
#include "WindowCreator.hpp"
#include "IChrono.hpp"
#include "TimerCreator.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"
#include "Event.hpp"
#include "ShaderProgram.hpp"
#include "VertexData.hpp"
#include "Matrix.hpp"
#include <iostream>
#include "Player.hpp"
#include "Stack.hpp"

int main()
{
    GLVM::Core::IWindow* Window = GLVM::Core::CWindowCreator().Create();
	GLVM::Time::IChrono* Chrono = GLVM::Time::CTimerCreator().Create();
	const char* iImage_Path = "../textures/hero.png";
	const char* iImage_Path2 = "../textures/enemy.png";
	GLVM::Core::CRenderer Renderer(vertices);
    GLVM::Core::CTexture Texture(iImage_Path);
	GLVM::Core::CTexture Texture2(iImage_Path2);
	GLVM::Core::CPlayer Player(Texture);
	GLVM::Core::CEvent Event;
    Shader Shader_Program("../GLVM/Shader.vs", "../GLVM/Shader.fs");
	GLVM::Math::TCMatrix<4> tMatrix2(0.1f);
	tMatrix2.Offset(-0.2f);
	double dDelta_Time = 0.0;
	bool bInput_Flag_A = false;
	bool bInput_Flag_D = false;
	bool bInput_Flag_S = false;
	bool bInput_Flag_W = false;
	bool bGame_Loop_Active = true;
	GLVM::Core::CStack Stack;
	
	
    ///< Game rendering loop
    while(bGame_Loop_Active)
    {
	    dDelta_Time = Chrono->GetElapsed();
		Chrono->Reset();
//		std::cout << dDelta_Time << std::endl;

		Window->ClearDisplay();
		Shader_Program.Use();
		Shader_Program.SetUniformID();
		
		Renderer.SetModelMatrix(&Shader_Program, tMatrix2.GetMatrix());
		Renderer.Draw(Texture2);
		
		while((Window->HandleEvent(Event)))
		{
			switch(Event.GetEvent())
			{
			case GLVM::Core::EEvents::eEXIT:
				bGame_Loop_Active = false;
				break;
			case GLVM::Core::EEvents::eKEYRELEASE_A:
				Stack.Remove(GLVM::Core::EEvents::eMOVE_LEFT);
				bInput_Flag_A = false;
				break;
			case GLVM::Core::EEvents::eKEYRELEASE_D:
				Stack.Remove(GLVM::Core::EEvents::eMOVE_RIGHT);
				bInput_Flag_D = false;
				break;
			case GLVM::Core::EEvents::eKEYRELEASE_S:
				Stack.Remove(GLVM::Core::EEvents::eMOVE_DOWN); 
				bInput_Flag_S = false;
				break;
			case GLVM::Core::EEvents::eKEYRELEASE_W:
				Stack.Remove(GLVM::Core::EEvents::eMOVE_UP); 
				bInput_Flag_W = false;
				break;
			case GLVM::Core::EEvents::eMOVE_LEFT:
				Stack.Push(GLVM::Core::EEvents::eMOVE_LEFT);
				bInput_Flag_A = true;
				break;
			case GLVM::Core::EEvents::eMOVE_RIGHT:
				Stack.Push(GLVM::Core::EEvents::eMOVE_RIGHT);
				bInput_Flag_D = true;
				break;
			case GLVM::Core::EEvents::eMOVE_DOWN:
				Stack.Push(GLVM::Core::EEvents::eMOVE_DOWN);
				bInput_Flag_S = true;
				break;
			case GLVM::Core::EEvents::eMOVE_UP:
				Stack.Push(GLVM::Core::EEvents::eMOVE_UP);
				bInput_Flag_W = true;
				break;
			}
		}
		std::cout << "Keycode " << Event.GetEvent() << std::endl;
		// if(bInput_Flag)
		// 	tMatrix3.Move(dDelta_Time, Event);
//		std::cout << bInput_Flag_A << " " << bInput_Flag_D <<
//			" " << bInput_Flag_S << " " << bInput_Flag_W << std::endl;
		if(bInput_Flag_A || bInput_Flag_D || bInput_Flag_S || bInput_Flag_W)
		{
//			std::cout << Event.GetEvent() << std::endl;
			// if(bInput_Flag_D)
			// 	Event.SetEvent(GLVM::Core::EEvents::eMOVE_RIGHT);
			// if(bInput_Flag_A)
			// 	Event.SetEvent(GLVM::Core::EEvents::eMOVE_LEFT);
			// if(bInput_Flag_S)
			// 	Event.SetEvent(GLVM::Core::EEvents::eMOVE_DOWN);
			// if(bInput_Flag_W)
			// 	Event.SetEvent(GLVM::Core::EEvents::eMOVE_UP);
			if(Stack.Pop() == GLVM::Core::eMOVE_RIGHT)
				Event.SetEvent(GLVM::Core::EEvents::eMOVE_RIGHT);
			if(Stack.Pop() == GLVM::Core::eMOVE_LEFT)
				Event.SetEvent(GLVM::Core::EEvents::eMOVE_LEFT);
			if(Stack.Pop() == GLVM::Core::eMOVE_DOWN)
				Event.SetEvent(GLVM::Core::EEvents::eMOVE_DOWN);
			if(Stack.Pop() == GLVM::Core::eMOVE_UP)
				Event.SetEvent(GLVM::Core::EEvents::eMOVE_UP);
			Player.Move(dDelta_Time, Event);
		}
		Stack.Show();
		Renderer.SetModelMatrix(&Shader_Program, Player.GetMatrix()->GetMatrix());
		Renderer.Draw(Texture);
		Window->SwapBuffers();
    }

	Window->Close();
    delete Window;
    Window = nullptr;
	delete Chrono;
	Chrono = nullptr;
    
    return 0;
}
