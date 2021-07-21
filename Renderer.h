#ifndef RENDERER
#define RENDERER

#include "Sprite.h"
#include "GLPointer.h"
#include <GL/gl.h>

namespace GLVM::Core
{    

    class CRenderer
    {
    
    public:
        CRenderer()
        {}

        void DrawSprite(int* shaderProgram, CSprite& Sprite);
    };

}
    
#endif
