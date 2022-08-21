#include "GraphicAPI/Opengl.hpp"
#include "ComponentManager.hpp"
#include "Engine.hpp"
#include "Event.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "GLPointer.h"
#include "VectorContainer.hpp"
#include "Components/VertexComponent.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include <GL/gl.h>
#include <cmath>
#include "Globals.hpp"

float fBase_Array[30] =
{
	// координаты        // текстурные координаты
	0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // верхняя правая вершина
	0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // нижняя правая вершина
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // нижняя левая вершина
	-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,  // верхняя левая вершина
	0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f
};

float aVertex_Box[VERTEX_ARRAY_RANGE] =
{
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

namespace GLVM::Core
{
    COpenglRenderer::COpenglRenderer()
	{
		_Shader_Program      = new Shader("../Shader.vs", "../Shader.fs");
        GUI_Shader_Program_ = new Shader("../GUIShader.vs", "../GUIShader.fs");
        
		// aMatrix_Ortho_[0]  = 2/1280.0f;
		// //Matrix_Ortho[3]  -= 1;
		// //Matrix_Ortho[7]  -= 1;
		// aMatrix_Ortho_[5]  = 2/1280.0f;
		// aMatrix_Ortho_[10] = 1/(600.0f-0.0f);
		// aMatrix_Ortho_[15] = 1.0f;
		// //Matrix_Ortho[14] = -m_zn/(m_zf-m_zn);

		float aVertices_[VERTEX_ARRAY_RANGE];
		
        for(int i = BASE_ARRAY_COUNTER_VALUE; i < VERTEX_ARRAY_RANGE; ++i)
            aVertices_[i] = aVertex_Box[i];
		
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
        glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, 1920, 1080);
	}

	COpenglRenderer::~COpenglRenderer()
	{
        delete _Shader_Program;
        _Shader_Program = nullptr;
        delete GUI_Shader_Program_;
        GUI_Shader_Program_ = nullptr;
        
		pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &iVao_);
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &iVbo_);
	}
    
	void COpenglRenderer::draw()
	{
        ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refTexture =
            ECS::GetInnerIDsContainer<ECS::CTextureComponent>(*pComponent_Manager);
        unsigned int uiVector_Texture_Size = pEntity_Container_refTexture->GetSize();

        _Shader_Program->Use();
        _Shader_Program->SetUniformID();
        
		for(int i = 0, iSize = uiVector_Texture_Size; i < iSize; ++i)
		{
            unsigned int uiEntity_refTexture= (*pEntity_Container_refTexture)[i];
			pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).aVertex_), &(pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).aVertex_), GL_DYNAMIC_DRAW);

            SetModelMatrix(_Shader_Program, pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
  			pGLActive_Texture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture).iTexture_);
			pGLBind_Vertex_Array(iVao_);

//            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                        
            glDrawArrays(GL_TRIANGLES, BASE_INDEX_VERTEX_ARRAY, NUMBER_OF_DROWING_VERTEXES);

//            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); 
		}
	}

	void COpenglRenderer::SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component)
	{
        Matrix<float, 4> tRotation_Matrix(1.0f);
        Matrix<float, 4> tModel_Matrix(1.0f);
        Matrix<float, 4> tScaling_Matrix(1.0f);
        Matrix<float, 4> tTranslation_Matrix(1.0f);

        tScaling_Matrix[0][0] = _transform_Component.fScale;
        tScaling_Matrix[1][1] = _transform_Component.fScale;
        tScaling_Matrix[2][2] = _transform_Component.fScale;
		tScaling_Matrix[3][3] = 1.0f;
        
        tTranslation_Matrix[3][0] = _transform_Component.tPosition[0];
		tTranslation_Matrix[3][1] = _transform_Component.tPosition[1];
		tTranslation_Matrix[3][2] = _transform_Component.tPosition[2];
        tTranslation_Matrix[3][3] = 1.0f;
        
        tModel_Matrix = tScaling_Matrix * tTranslation_Matrix;
//		tModel_Matrix = tTranslation_Matrix * tScaling_Matrix;
        
        unsigned int uiTransformt_Loc = pGLGet_Uniform_Location(_Shader_Program->iID, "aModel_Matrix");
		pGLUniform_Matrix4fv(uiTransformt_Loc, NUMBER_OF_MATRICES, GL_FALSE, &tModel_Matrix[0][0]);
	}
}
