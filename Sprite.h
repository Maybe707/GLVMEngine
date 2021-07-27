#ifndef SPRITE
#define SPRITE

#include <GL/gl.h>
#include "GLPointer.h"

namespace GLVM::Core
{

    /*! \class Sprite
        \brief Creates sprites for game objects.
    */
    
    class CSprite
    {
        GLuint iVbo_;
        GLuint iVao_;
        float aVertices_[9];
     
    public:
        CSprite(float* _aVertices);
        ~CSprite();
        
        GLuint* GetVbo();
        GLuint* GetVao();
    };
}
    
#endif
