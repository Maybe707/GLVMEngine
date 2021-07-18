#include "IWindow.h"
#include "WindowCreator.h"
#include "LinuxWindow.h"
#include "GLPointer.h"
#include <GL/glext.h>
#include <X11/Xlib.h>
#include <iostream>

// Константы
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
 
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

void GLMainFunction(unsigned int& iVao, unsigned int& iVbo, float* aVertices, const int iVerticesSize)
{
    pGLGen_Vertex_Arrays(1, &iVao);
    pGLGen_Buffers(1, &iVbo);
 
    // Сначала связываем объект вершинного массива, затем связываем и устанавливаем вершинный буфер(ы), и затем конфигурируем вершинный атрибут(ы)
    pGLBind_Vertex_Array(iVao);
 
    pGLBind_Buffer(GL_ARRAY_BUFFER, iVbo);
    pGLBuffer_Data(GL_ARRAY_BUFFER, iVerticesSize, aVertices, GL_STATIC_DRAW);
 
    pGLVertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    pGLEnable_Vertex_Attrib_Array(0);
 
    // Обратите внимание, что данное действие разрешено, вызов glVertexAttribPointer() зарегистрировал VBO как привязанный вершинный буферный объект для вершинного атрибута, так что после этого мы можем спокойно выполнить отвязку
    pGLBind_Buffer(GL_ARRAY_BUFFER, 0);
 
    // Вы можете отменить привязку VAO после этого, чтобы другие вызовы VAO случайно не изменили этот VAO (но подобное довольно редко случается).
    // Модификация других VAO требует вызова glBindVertexArray(), поэтому мы обычно не снимаем привязку VAO (или VBO), когда это не требуется напрямую
    pGLBind_Vertex_Array(0);
}

int main()
{
    GLVM::Core::IWindow* Window = GLVM::Core::CWindowCreator().Create();
    
//    Window->EventLoop();

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

    // Указывание вершин (и буферов) и настройка вершинных атрибутов
    float aVertices[] =
    {
        -0.5f, -0.5f, 0.0f, // левая вершина
         0.5f, -0.5f, 0.0f, // правая вершина
         0.0f,  0.5f, 0.0f  // верхняя вершина   
    };
    const int iVerticesSize = sizeof(aVertices);

    unsigned int iVbo, iVao;
    // Раскомментируйте следующую строку для отрисовки полигонов в режиме каркаса
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    GLMainFunction(iVao, iVbo, aVertices, iVerticesSize);
    
    XWindowAttributes Gwa;
    XEvent Xevent;
    
    // Цикл рендеринга
    while (true)
    {
 
        // glfw: обмен содержимым front- и back-буферов. Отслеживание событий ввода/вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
//        glfwSwapBuffers(window);
//        glfwPollEvents();

        
        XNextEvent(Window->GetDisplay(), &Xevent);
        
        if(Xevent.type == Expose)
        {
            XGetWindowAttributes(Window->GetDisplay(), Window->GetWindow(), &Gwa);
            glViewport(0, 0, Gwa.width, Gwa.height);

            // Рендеринг
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
 
            // Рисуем наш первый треугольник
            pGLUse_Program(shaderProgram);
            pGLBind_Vertex_Array(iVao); // поскольку у нас есть только один VAO, то нет необходимости связывать его каждый раз (но мы сделаем это, чтобы всё было немного организованнее)
            glDrawArrays(GL_TRIANGLES, 0, 3);
            // glBindVertexArray(0); // не нужно каждый раз его отвязывать

            
            glXSwapBuffers(Window->GetDisplay(), Window->GetWindow());
        }
                
        else if(Xevent.type == KeyPress)
        {
            glXMakeCurrent(Window->GetDisplay(), None, NULL);
            glXDestroyContext(Window->GetDisplay(), Window->GetContext());
            XDestroyWindow(Window->GetDisplay(), Window->GetWindow());
            XCloseDisplay(Window->GetDisplay());
            exit(0);
        }
    }
 
    // Опционально: освобождаем все ресурсы, как только они выполнили свое предназначение
    pGLDelete_Vertex_Arrays(1, &iVao);
    pGLDelete_Buffers(1, &iVbo);
    
    return 0;
}
