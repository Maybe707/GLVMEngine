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
    GLVM::Core::CSprite Sprite(aVertices);
    GLVM::Core::CSprite Sprite2(aVertices2);
    GLVM::Core::CRenderer Renderer;
    GLVM::Core::CEvent Event;
    Shader Shader_Program("../GLVM/Shader.vs", "../GLVM/Shader.fs");
    
    ///< Game rendering loop
    while (true)
    {
        Window->ClearDisplay();
        Shader_Program.Use();
        Renderer.DrawSprite(Sprite);
        Renderer.DrawSprite(Sprite2);

        Window->HandleEvent(Event);
        if(Event.GetEvent() == GLVM::Core::EEvents::eEXIT)
            Window->Close();
        Window->SwapBuffers();
        if(Event.GetReturnFlag())
            break;
    }
    
    delete Window;
    Window = nullptr;
    
    return 0;
}
