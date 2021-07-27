#include "Sprite.h"
#include <KHR/khrplatform.h>

namespace GLVM::Core
{    
    CSprite::CSprite(float* _aVertices)
    {
        for(int i = 0; i < 9; ++i)
            aVertices_[i] = _aVertices[i];
            
        pGLGen_Vertex_Arrays(1, &iVao_);
        pGLGen_Buffers(1, &iVbo_);
 
        ///< First we link the vertex array object, then we link and set the vertex buffers, and then we configure the vertex attributes.
        
        pGLBind_Vertex_Array(iVao_);
 
        pGLBind_Buffer(GL_ARRAY_BUFFER, iVbo_);
        pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(aVertices_), aVertices_, GL_STATIC_DRAW);
 
        pGLVertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        pGLEnable_Vertex_Attrib_Array(0);

        /********************************************************************
         * Note that this action is allowed, the glVertexAttribPointer () call
         * has registered the VBO as an anchored vertex buffer for the vertex
         * attribute, so we can safely unbind after that.
         *******************************************************************/

        pGLBind_Buffer(GL_ARRAY_BUFFER, 0);

        /***********************************************************************
         * You can unbind a VAO afterwards so that other VAO calls don't
         * accidentally change this VAO (but this rarely happens).
         * Modifying other VAOs requires a call to glBindVertexArray (),
         * so we usually don't unbind a VAO (or VBO) when not directly required.
         **********************************************************************/

        pGLBind_Vertex_Array(0);
    }

    GLuint* CSprite::GetVbo()
    {
        return &iVbo_;
    }

    GLuint* CSprite::GetVao()
    {
        return &iVao_;
    }

    CSprite::~CSprite()
    {
        pGLDelete_Vertex_Arrays(1, &iVao_);
        pGLDelete_Buffers(1, &iVbo_);
    }
}
