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

#define ORTO_MATRIX_RANGE 16

namespace GLVM::Core
{    
    class CRenderer
    {
	    GLuint iVbo_;
		GLuint iVao_;
		float aMatrix_Ortho_[ORTO_MATRIX_RANGE] = {};
    
    public:
        CRenderer();
		~CRenderer();

        void Draw(CPlayer& _Player);
		void SetModelMatrix(Shader* _Shader_Program, float const* _Model_Matrix);
		void DrawAll(TCVectorContainer<GameObject*>* pWorldContainer, Shader* _Shader_Program);
		void SetProjectionMatrix(Shader* _Shader_Program);
    };
}
    
#endif
