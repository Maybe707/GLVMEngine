#include "RenderSystem.hpp"
#include "ComponentManager.hpp"
#include "ConstVectorContainer.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include "AnimationMoveComponent.hpp"
#include "VectorContainer.hpp"

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
#define NUMBER_OF_MATRICES 1

float fBase_Array[VERTEX_ARRAY_RANGE] =
{
	// координаты        // текстурные координаты
	0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // верхняя правая вершина
	0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // нижняя правая вершина
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // нижняя левая вершина
	-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,  // верхняя левая вершина
	0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f
};

namespace GLVM::Core
{
    CRenderSystem::CRenderSystem()
	{
		aMatrix_Ortho_[0]  = 2/1280.0f;
		//Matrix_Ortho[3]  -= 1;
		//Matrix_Ortho[7]  -= 1;
		aMatrix_Ortho_[5]  = 2/1280.0f;
		aMatrix_Ortho_[10] = 1/(600.0f-0.0f);
		aMatrix_Ortho_[15] = 1.0f;
		//Matrix_Ortho[14] = -m_zn/(m_zf-m_zn);
		float aVertices_[VERTEX_ARRAY_RANGE];
		
        for(int i = BASE_ARRAY_COUNTER_VALUE; i < VERTEX_ARRAY_RANGE; ++i)
            aVertices_[i] = fBase_Array[i];
		
		pGLGen_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &iVao_);
        pGLGen_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &iVbo_);
 
        ///< First we link the vertex array object, then we link and set the vertex buffers, and then we configure the vertex attributes.
        
        pGLBind_Vertex_Array(iVao_);

		pGLBind_Buffer(GL_ARRAY_BUFFER, iVbo_);
        pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(aVertices_), aVertices_, GL_DYNAMIC_DRAW);
 
        pGLVertex_Attrib_Pointer(LAYOUT_0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, SIZE_OF_VERTEX_DATA * sizeof(float), (void*)VERTEX_OFFSET);
        pGLEnable_Vertex_Attrib_Array(LAYOUT_0);
		pGLVertex_Attrib_Pointer(LAYOUT_1, TEXTURE_SIZE, GL_FLOAT, GL_FALSE, SIZE_OF_VERTEX_DATA * sizeof(float), (void*)(TEXTURE_OFFSET * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(LAYOUT_1);
///<	stbi_set_flip_vertically_on_load(true);
		glViewport(0, 0, 1280, 1280);
	}

	CRenderSystem::~CRenderSystem()
	{
		pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &iVao_);
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &iVbo_);
	}

    void CRenderSystem::Draw()
    {
		pGLActive_Texture(GL_TEXTURE10);
//		glBindTexture(GL_TEXTURE_2D, _Player.GetTexture().GetTexture());
		pGLBind_Vertex_Array(iVao_);
        glDrawArrays(GL_TRIANGLES, BASE_INDEX_VERTEX_ARRAY, NUMBER_OF_DROWING_VERTEXES);
    }

	void CRenderSystem::DrawAll(TCConstVectorContainer<ECS::STransformComponent>* _tTransformContainer,
								TCConstVectorContainer<ECS::CTextureComponent>* _tTextureContainer,
								TCConstVectorContainer<ECS::SVertexComponent>* _pVertex_Container,
								TCVectorContainer<unsigned int>* _pOrdered_Vertex_Container,
								Shader* _Shader_Program)
	{
		for(int i = 0; i < _tTransformContainer->GetSize(); ++i)
		{
			pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof((*_pVertex_Container)[(*_pOrdered_Vertex_Container)[i]].aVertex_), &(*_pVertex_Container)[(*_pOrdered_Vertex_Container)[i]].aVertex_, GL_DYNAMIC_DRAW);
			SetModelMatrix(_Shader_Program, (*_tTransformContainer)[i]);
			pGLActive_Texture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, (*_tTextureContainer)[i].iTexture_);
			pGLBind_Vertex_Array(iVao_);
			glDrawArrays(GL_TRIANGLES, BASE_INDEX_VERTEX_ARRAY, NUMBER_OF_DROWING_VERTEXES);
		}
	}
 
	void CRenderSystem::SetModelMatrix(Shader* _Shader_Program, const ECS::STransformComponent& _transform_Component)
	{
		Model_Matrix_.Offset(_transform_Component);
		Model_Matrix_.Matrix()[0][0] = 64;
		Model_Matrix_.Matrix()[1][1] = 64;
		Model_Matrix_.Matrix()[2][2] = 64;
		unsigned int uiTransformt_Loc = pGLGet_Uniform_Location(_Shader_Program->iID, "aModel_Matrix");
		pGLUniform_Matrix4fv(uiTransformt_Loc, NUMBER_OF_MATRICES, GL_FALSE, Model_Matrix_.GetMatrix());
	}

	void CRenderSystem::SetProjectionMatrix(Shader* _Shader_Program)
	{
		unsigned int uiTransformt = pGLGet_Uniform_Location(_Shader_Program->iID, "aProjection_Matrix");
		pGLUniform_Matrix4fv(uiTransformt, NUMBER_OF_MATRICES, GL_FALSE, aMatrix_Ortho_);
	}
}
