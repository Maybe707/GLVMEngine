#ifndef SPRITE
#define SPRITE

#include <GL/gl.h>
#include "GLPointer.h"

namespace GLVM::Core
{    
    class CSprite
    {
        GLuint iVbo_;
        GLuint iVao_;
        float aVertices[12];
     
    public:
        CSprite(float* aVerticesP);
        ~CSprite();
        
        GLuint* GetVbo();
        GLuint* GetVao();
    };
}
    
#endif
