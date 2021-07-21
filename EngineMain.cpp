#include "IWindow.h"
#include "WindowCreator.h"
#include "WindowLin.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <X11/Xlib.h>
#include <iostream>
#include "GLPointer.h"
#include "Sprite.h"
#include "Renderer.h"

// Константы
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
 
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   mat4 m;"
"    m[0] = vec4(1, 0, 0, 0);"
"    m[1] = vec4(0, 1, 0, 0);"
"    m[2] = vec4(0, 0, 1, 0);"
"    m[3] = vec4(0.2, 0.2, 0.2, 1);"
"   gl_Position = m * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
    GLVM::Core::IWindow* Window = GLVM::Core::CWindowCreator().Create();
    
    // Вершинный шейдер
    int vertexShader = pGLCreate_Shader(GL_VERTEX_SHADER);
    pGLShader_Source(vertexShader, 1, &vertexShaderSource, NULL);
    pGLCompile_Shader(vertexShader);

    // Проверка на наличие ошибок компилирования вершинного шейдера
    int success;
    char infoLog[512];
    pGLGet_Shaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        pGLGet_Shader_Info_Log(vertexShader, 512, 0, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Фрагментный шейдер
    int fragmentShader = pGLCreate_Shader(GL_FRAGMENT_SHADER);
    pGLShader_Source(fragmentShader, 1, &fragmentShaderSource, NULL);
    pGLCompile_Shader(fragmentShader);
 
    // Проверка на наличие ошибок компилирования фрагментного шейдера
    pGLGet_Shaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        pGLGet_Shader_Info_Log(fragmentShader, 512, 0, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Связывание шейдеров
    int shaderProgram = pGLCreate_Program();
    pGLAttach_Shader(shaderProgram, vertexShader);
    pGLAttach_Shader(shaderProgram, fragmentShader);
    pGLLink_Program(shaderProgram);
 
    // Проверка на наличие ошибок компилирования связывания шейдеров
    pGLGet_Programiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        pGLGet_Program_Info_Log(shaderProgram, 512, 0, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    pGLDelete_Shader(vertexShader);
    pGLDelete_Shader(fragmentShader);

    float aVertices[12] =
    {
        -0.5f, -0.5f, -0.5f, // левая вершина
         0.5f, -0.5f, 0.5f, // правая вершина
         0.0f,  -1.0f, -0.5f  // верхняя вершина   
    };

    float aVertices2[12] =
    {
        -0.5f, -0.5f, -0.5f, // левая вершина
         0.5f, -0.5f, 0.5f, // правая вершина
         1.0f,  -1.0f, -1.0f  // верхняя вершина   
    };
    
    CSprite Sprite(aVertices);
    CSprite Sprite2(aVertices2);
    GLVM::Core::CRenderer Renderer;
    
    XEvent Xevent;

    // Цикл рендеринга
    while (true)
    {
      XNextEvent(Window->GetDisplay(), &Xevent);
        
      Renderer.DrawSprite(&shaderProgram, Sprite);
      Renderer.DrawSprite(&shaderProgram, Sprite2);
      Window->SwapBuffers();

      if(Xevent.type == KeyPress)
      {
          glXMakeCurrent(Window->GetDisplay(), None, NULL);
          glXDestroyContext(Window->GetDisplay(), Window->GetContext());
          XDestroyWindow(Window->GetDisplay(), Window->GetWindow());
          XCloseDisplay(Window->GetDisplay());
          exit(0);
      }
    }

    
    return 0;
}
