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
	    GLuint iVbo_;
		GLuint iVao_;
    
    public:
        CRenderer(float* _aVertices);
		~CRenderer();

        void Draw();
		void SetModelMatrix(Shader* _Shader_Program, float* _Model_Matrix);
    };
}
    
#endif
