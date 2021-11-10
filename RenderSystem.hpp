#ifndef RENDERER
#define RENDERER

#include "TextureComponent.hpp"
#include "GLPointer.h"
#include <GL/gl.h>
#include "VectorContainer.hpp"
#include "VertexComponent.hpp"
#include "ComponentManager.hpp"
#include "ConstVectorContainer.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include <GL/glext.h>
#include "AnimationMoveComponent.hpp"

/*! \class Renderer.
    \brief Render all game objects.

    Take a game object to render in DrawSprite method.
*/

#define MATRIX_RANGE 16
#define VERTEX_ARRAY_RANGE 30
#define SIZE_OF_VERTEX_DATA 5
#define LAYOUT_0 0
#define LAYOUT_1 1
#define VERTEX_SIZE 3
#define TEXTURE_SIZE 2
#define VERTEX_OFFSET 0
#define TEXTURE_OFFSET 3
#define NUMBER_OF_CREATING_VBO_OBJECT_1 1
#define NUMBER_OF_CREATING_VAO_OBJECT_1 1
#define BASE_ARRAY_COUNTER_VALUE 0
#define BASE_INDEX_VERTEX_ARRAY 0
#define NUMBER_OF_DROWING_VERTEXES 6
#define HOMOGENEOUS_COORDINATE 1
#define NUMBER_OF_MATRICES 1
#define LIMITER 1

namespace GLVM::ECS
{    
    class CRenderSystem
    {
	    GLuint iVbo_;
		GLuint iVao_;
		float aMatrix_Ortho_[MATRIX_RANGE] = {};
		static const unsigned int u_iRange = 4;
		float aMatrix_Model_[u_iRange][u_iRange] {};
    
    public:
        CRenderSystem();
		~CRenderSystem();

		void LoadTextureData(ECS::CTextureComponent& _Texture);
		void SetModelMatrix(Shader* _Shader_Program, const ECS::STransformComponent& _transform_Component);
		void DrawAll(Core::TCConstVectorContainer<ECS::STransformComponent>* _tTransformContainer,
					 Core::TCConstVectorContainer<ECS::CTextureComponent>* _tTextureContainer,
					 Core::TCConstVectorContainer<ECS::SVertexComponent>* _pVertex_Container,
					 Core::TCVectorContainer<unsigned int>* _pOrdered_Vertex_Container,
					 Core::TCVectorContainer<unsigned int>* _pOrdered_Texture_Container,
					 Shader* _Shader_Program);
		void SetProjectionMatrix(Shader* _Shader_Program);
    };
}
    
#endif
