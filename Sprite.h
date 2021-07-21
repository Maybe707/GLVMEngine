#ifndef SPRITE
#define SPRITE

#include <GL/gl.h>
#include "GLPointer.h"

class CSprite
{
    GLuint iVbo_;
    GLuint iVao_;
    float aVertices[12];
     
public:
    CSprite(float* aVerticesP)
    {
        for(int i = 0; i < 12; ++i)
            aVertices[i] = aVerticesP[i];
            
        pGLGen_Vertex_Arrays(1, &iVao_);
        pGLGen_Buffers(1, &iVbo_);
 
        // Сначала связываем объект вершинного массива, затем связываем и устанавливаем вершинный буфер(ы), и затем конфигурируем вершинный атрибут(ы)
        pGLBind_Vertex_Array(iVao_);
 
        pGLBind_Buffer(GL_ARRAY_BUFFER, iVbo_);
        pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(aVertices), aVertices, GL_STATIC_DRAW);
 
        pGLVertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        pGLEnable_Vertex_Attrib_Array(0);
    
        // Обратите внимание, что данное действие разрешено, вызов glVertexAttribPointer() зарегистрировал VBO как привязанный вершинный буферный объект для вершинного атрибута, так что после этого мы можем спокойно выполнить отвязку
        pGLBind_Buffer(GL_ARRAY_BUFFER, 0);
 
        // Вы можете отменить привязку VAO после этого, чтобы другие вызовы VAO случайно не изменили этот VAO (но подобное довольно редко случается).
        // Модификация других VAO требует вызова glBindVertexArray(), поэтому мы обычно не снимаем привязку VAO (или VBO), когда это не требуется напрямую
        pGLBind_Vertex_Array(0);
     }

    GLuint* GetVbo()
    {
        return &iVbo_;
    }

    GLuint* GetVao()
    {
        return &iVao_;
    }

    ~CSprite()
    {
        pGLDelete_Vertex_Arrays(1, &iVao_);
        pGLDelete_Buffers(1, &iVbo_);
    }
};

#endif
