#ifndef RENDERER
#define RENDERER

#include "Sprite.h"
#include "GLPointer.h"
#include <GL/gl.h>

/*! \class Renderer.
    \brief Render all game objects.

    Take a game object to render in DrawSprite method.
*/

namespace GLVM::Core
{    

    class CRenderer
    {
    
    public:
        CRenderer();

        void DrawSprite(CSprite& _Sprite);
    };

}
    
#endif
