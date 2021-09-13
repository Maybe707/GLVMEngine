#include "GLPointer.h"
#include "IWindow.hpp"
#include "WindowCreator.hpp"
#include "Texture.hpp"
#include "Renderer.hpp"
#include "Event.hpp"
#include "ShaderProgram.hpp"
#include "VertexData.hpp"
#include "Matrix.hpp"

int main()
{
    GLVM::Core::IWindow* Window = GLVM::Core::CWindowCreator().Create();
	const char* iImage_Path = "../textures/hero.png";
	const char* iImage_Path2 = "../textures/enemy.png";
	GLVM::Core::CRenderer Renderer(vertices);
    GLVM::Core::CTexture Texture(iImage_Path);
	GLVM::Core::CTexture Texture2(iImage_Path2);
    GLVM::Core::CEvent Event;
    Shader Shader_Program("../GLVM/Shader.vs", "../GLVM/Shader.fs");
	GLVM::Math::TCMatrix<4> tMatrix(0.1f);
	GLVM::Math::TCMatrix<4> tMatrix2(0.1f);
	tMatrix.Offset(0.6f);
	tMatrix2.Offset(-0.2f);
	
    ///< Game rendering loop
    while (true)
    {
        Window->ClearDisplay();
		Shader_Program.Use();
		Shader_Program.SetUniformID();

		Renderer.SetModelMatrix(&Shader_Program, tMatrix.GetMatrix());
        Renderer.Draw(Texture);
		Renderer.SetModelMatrix(&Shader_Program, tMatrix2.GetMatrix());
		Renderer.Draw(Texture2);
        Window->SwapBuffers();
        Window->HandleEvent(Event);
        if(Event.GetEvent() == GLVM::Core::EEvents::eEXIT)
        {
            Window->Close();
            break;
        }
    }
    
    delete Window;
    Window = nullptr;
    
    return 0;
}
