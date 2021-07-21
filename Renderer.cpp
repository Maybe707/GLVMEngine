#include "Renderer.h"

namespace GLVM::Core
{    

    void CRenderer::DrawSprite(int* shaderProgram, CSprite& Sprite)
    {
        // Рендеринг
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 
        // Рисуем наш первый треугольник
        pGLUse_Program(*shaderProgram);
        pGLBind_Vertex_Array(*Sprite.GetVao()); // поскольку у нас есть только один VAO, то нет необходимости связывать его каждый раз (но мы сделаем это, чтобы всё было немного организованнее)
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // не нужно каждый раз его отвязывать
    }

}
