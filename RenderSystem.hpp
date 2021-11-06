#ifndef RENDERER
#define RENDERER

#include "TextureComponent.hpp"
#include "GLPointer.h"
#include <GL/gl.h>
#include "VectorContainer.hpp"
#include "MatrixComponent.hpp"
#include "VertexComponent.hpp"

/*! \class Renderer.
    \brief Render all game objects.

    Take a game object to render in DrawSprite method.
*/

#define MATRIX_RANGE 16

namespace GLVM::Core
{    
    class CRenderSystem
    {
	    GLuint iVbo_;
		GLuint iVao_;
		float aMatrix_Ortho_[MATRIX_RANGE] = {};
		ECS::CMatrixComponent Model_Matrix_;
    
    public:
        CRenderSystem();
		~CRenderSystem();

		void LoadTextureData(ECS::CTextureComponent& _Texture);
        void Draw();
		void SetModelMatrix(Shader* _Shader_Program, const ECS::STransformComponent& _transform_Component);
		void DrawAll(TCConstVectorContainer<ECS::STransformComponent>* _tTransformContainer,
								TCConstVectorContainer<ECS::CTextureComponent>* _tTextureContainer,
								Core::TCConstVectorContainer<ECS::SVertexComponent>* _pVertex_Container,
								Core::TCVectorContainer<unsigned int>* _pOrdered_Vertex_Container,
								Shader* _Shader_Program);
		void SetProjectionMatrix(Shader* _Shader_Program);
    };
}
    
#endif
