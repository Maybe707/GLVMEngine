#ifndef RENDERER
#define RENDERER

#include "Texture.hpp"
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

        void Draw(CTexture const& _Sprite);
		void SetModelMatrix(Shader* _Shader_Program, float const* _Model_Matrix);
    };
}
    
#endif
