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

int main()
{
    GLVM::Core::IWindow* Window = GLVM::Core::CWindowCreator().Create();
	GLVM::Time::IChrono* Chrono = GLVM::Time::CTimerCreator().Create();
	const char* iImage_Path = "../textures/hero.png";
	const char* iImage_Path2 = "../textures/enemy.png";
	GLVM::Core::CRenderer Renderer(vertices);
    GLVM::Core::CTexture Texture(iImage_Path);
	GLVM::Core::CTexture Texture2(iImage_Path2);
	GLVM::Core::CEvent Event;
    Shader Shader_Program("../GLVM/Shader.vs", "../GLVM/Shader.fs");
	GLVM::Math::TCMatrix<4> tMatrix(0.1f);
	GLVM::Math::TCMatrix<4> tMatrix2(0.1f);
	GLVM::Math::TCMatrix<4> tMatrix3(0.1f);
	tMatrix.Offset(0.6f);
	tMatrix2.Offset(-0.2f);
	tMatrix3.Offset(0.5f);
	double dDelta_Time = 0.0;
	bool bInput_Flag = false;
	bool bGame_Loop_Active = true;
	
    ///< Game rendering loop
    while(bGame_Loop_Active)
    {
	    dDelta_Time = Chrono->GetElapsed();
		Chrono->Reset();
		std::cout << dDelta_Time << std::endl;
 		
		Window->ClearDisplay();
		Shader_Program.Use();
		Shader_Program.SetUniformID();
		
		Renderer.SetModelMatrix(&Shader_Program, tMatrix2.GetMatrix());
		Renderer.Draw(Texture2);
		
		Window->HandleEvent(Event);
		switch(Event.GetEvent())
		{
		case GLVM::Core::EEvents::eEXIT:
			bGame_Loop_Active = false;
			break;
		case GLVM::Core::EEvents::eKEYRELEASE:
			bInput_Flag = false;
			break;
		case GLVM::Core::EEvents::eMOVE_LEFT:
			bInput_Flag = true;
			break;
		case GLVM::Core::EEvents::eMOVE_RIGHT:
			bInput_Flag = true;
			break;
		case GLVM::Core::EEvents::eMOVE_DOWN:
			bInput_Flag = true;
			break;
		case GLVM::Core::EEvents::eMOVE_UP:
			bInput_Flag = true;
			break;
		}
		
		if(bInput_Flag)
			tMatrix3.Move(dDelta_Time, Event);
		Renderer.SetModelMatrix(&Shader_Program, tMatrix3.GetMatrix());
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
