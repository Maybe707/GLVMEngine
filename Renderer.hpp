#ifndef RENDERER
#define RENDERER

#include "Player.hpp"
#include "Texture.hpp"
#include "GLPointer.h"
#include <GL/gl.h>
#include "VectorContainer.hpp"

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
        CRenderer();
		~CRenderer();

        void Draw(CPlayer& _Player);
		void SetModelMatrix(Shader* _Shader_Program, float const* _Model_Matrix);
		void DrawAll(TCVectorContainer<IGameObject*>* pWorldContainer, Shader* _Shader_Program);
    };
}
    
#endif
