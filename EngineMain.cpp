#include "GLPointer.h"
#include "IWindow.h"
#include "WindowCreator.h"
#include "Sprite.h"
#include "Renderer.h"
#include "Event.h"
#include "ShaderProgram.h"
#include "VertexData.h"

int main()
{
    GLVM::Core::IWindow* Window = GLVM::Core::CWindowCreator().Create();
	const char* iImage_Path = "../textures/hero.png";
	const char* iImage_Path2 = "../textures/enemy.png";
	GLVM::Core::CRenderer Renderer(vertices);
    GLVM::Core::CSprite Sprite(iImage_Path);
	GLVM::Core::CSprite Sprite2(iImage_Path2);
    GLVM::Core::CEvent Event;
    Shader Shader_Program("../GLVM/Shader.vs", "../GLVM/Shader.fs");
	float aModel_Matrix[4][4] = {{0.1f, 0.0f, 0.0f, 0.2f},
								 {0.0f, 0.1f, 0.0f, 0.2f},
								 {0.0f, 0.0f, 0.1f, 0.2f},
								 {0.0f, 0.0f, 0.0f, 1.0f}};
	float aModel_Matrix2[4][4] = {{0.1f, 0.0f, 0.0f, 0.3f},
								 {0.0f, 0.1f, 0.0f, 0.3f},
								 {0.0f, 0.0f, 0.1f, 0.3f},
								 {0.0f, 0.0f, 0.0f, 1.0f}};
		
    ///< Game rendering loop
    while (true)
    {
        Window->ClearDisplay();
		Shader_Program.Use();
 		pGLUniform1i(pGLGet_Uniform_Location(Shader_Program.iID, "tex"), 10);

		Renderer.SetModelMatrix(&Shader_Program, *aModel_Matrix);
		Sprite.ActiveTexture();
		Sprite.BindTexture();
        Renderer.Draw();
		Renderer.SetModelMatrix(&Shader_Program, *aModel_Matrix2);
		Sprite2.ActiveTexture();
		Sprite2.BindTexture();
		Renderer.Draw();
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
