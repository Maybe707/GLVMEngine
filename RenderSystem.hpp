#ifndef RENDERER
#define RENDERER

#include "GLPointer.h"
#include <GL/gl.h>
#include "ISystem.hpp"
#include "VectorContainer.hpp"
#include "VertexComponent.hpp"
#include "ComponentManager.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include <GL/glext.h>
#include "AnimationMoveComponent.hpp"
#include "VertexMath.hpp"

/*! \class Renderer.
    \brief Render all game objects.

    Take a game object to render in DrawSprite method.
*/

#define MATRIX_RANGE 16
#define VERTEX_ARRAY_RANGE 180
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
#define NUMBER_OF_DROWING_VERTEXES 36
#define HOMOGENEOUS_COORDINATE 1
#define NUMBER_OF_MATRICES 1
#define LIMITER 1
#define PI 3.14159265

namespace GLVM::ECS
{    
    class CRenderSystem : public ISystem
    {
	public:
	    GLuint iVbo_;
		GLuint iVao_;
		static const unsigned int u_iRange = 4;
        Matrix<float, 4> tProjection_Matrix{1.0f};
		Shader* _Shader_Program;

        CRenderSystem();
		~CRenderSystem();

		void LoadTextureData(ECS::CTextureComponent& _Texture);
		void Update(CComponentManager& _Component_Manager) override;
		void SetProjectionMatrix(Shader* _Shader_Program);
        void SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component);
        void SetViewMatrix(Shader* _Shader_Program, const ECS::STransformComponent& _transform_Component);
        void PrintMatrix(Matrix<float, 4> _tMatrix);
    };
}
    
#endif
