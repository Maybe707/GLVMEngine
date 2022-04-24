#include "RenderSystem.hpp"
#include "ComponentManager.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include "VertexComponent.hpp"
#include "VertexMath.hpp"
#include "ViewComponent.hpp"
#include <GL/gl.h>
#include <cmath>

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

namespace GLVM::ECS
{
    CRenderSystem::CRenderSystem()
	{
		for(unsigned int count = 0; count < (u_iRange-LIMITER); ++count)
		{
			aMatrix_Model_[count][count] = 64.0;
		}
		aMatrix_Model_[u_iRange-LIMITER][u_iRange-LIMITER] = HOMOGENEOUS_COORDINATE;
		
		// aMatrix_Ortho_[0]  = 2/1280.0f;
		// //Matrix_Ortho[3]  -= 1;
		// //Matrix_Ortho[7]  -= 1;
		// aMatrix_Ortho_[5]  = 2/1280.0f;
		// aMatrix_Ortho_[10] = 1/(600.0f-0.0f);
		// aMatrix_Ortho_[15] = 1.0f;
		// //Matrix_Ortho[14] = -m_zn/(m_zf-m_zn);

        // aMatrix_Ortho_[0][0] = 2/1280.0f;
        // aMatrix_Ortho_[1][1] = 2/1280.0f;
        // aMatrix_Ortho_[2][2] = 1/(600.0f - 0.0f);
        // aMatrix_Ortho_[3][3] = 1.0f;

        float fov = 90;
        float f = 3, n = 10;
        float S = std::tan((fov/2)*(PI/360));
        aMatrix_Ortho_[0][0] = 1/((1280.0f/1280.0f)*S);
        aMatrix_Ortho_[1][1] = 1/S;
        aMatrix_Ortho_[2][2] = -((f+n)/(f-n));
        aMatrix_Ortho_[2][3] = -((2*f*n)/(f-n));
        aMatrix_Ortho_[3][2] = -1;
        aMatrix_Ortho_[3][3] = 1;

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
///<	stbi_set_flip_vertically_on_load(true);
        glEnable(GL_DEPTH_TEST);
		glViewport(640, 0, 1920, 1920);
	}

	CRenderSystem::~CRenderSystem()
	{
		pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &iVao_);
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &iVbo_);
	}

	void CRenderSystem::Update(CComponentManager& _Component_Manager)
	{
		Core::TCVectorContainer<STransformComponent>* _tTransformContainer = GetInnerComponentContainer<STransformComponent>(_Component_Manager);
		Core::TCVectorContainer<CTextureComponent>* _tTextureContainer = GetInnerComponentContainer<CTextureComponent>(_Component_Manager);
		Core::TCVectorContainer<SVertexComponent>* _pVertex_Container = GetInnerComponentContainer<SVertexComponent>(_Component_Manager);
        Core::TCVectorContainer<CViewComponent>* _pView_Container = GetInnerComponentContainer<CViewComponent>(_Component_Manager);
        Core::TCVectorContainer<unsigned int>* _pOrdered_View_Container = GetInnerIDsContainer<CViewComponent>(_Component_Manager);
		Core::TCVectorContainer<unsigned int>* _pOrdered_Texture_Container = GetInnerIDsContainer<CTextureComponent>(_Component_Manager);
        for(int j = 0, iSize = _pOrdered_View_Container->GetSize(); j < iSize; ++j)
        {
            SetViewMatrix(_Shader_Program, (*_tTransformContainer)[(*_pOrdered_View_Container)[j]]);
        }
		for(int i = 0, iSize = _pOrdered_Texture_Container->GetSize(); i < iSize; ++i)
		{
            (*_tTransformContainer)[(*_pOrdered_Texture_Container)[0]].fRotate += 1;
			pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof((*_pVertex_Container)[(*_pOrdered_Texture_Container)[i]].aVertex_), &(*_pVertex_Container)[(*_pOrdered_Texture_Container)[i]].aVertex_, GL_DYNAMIC_DRAW);

//            SetModelMatrix(_Shader_Program, (*_tTransformContainer)[(*_pOrdered_Texture_Container)[i]]);
            
            if(i == 0)
                SetRotate(_Shader_Program, (*_tTransformContainer)[(*_pOrdered_Texture_Container)[i]]);
            else 
                SetModelMatrix(_Shader_Program, (*_tTransformContainer)[(*_pOrdered_Texture_Container)[i]]);

			pGLActive_Texture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, (*_tTextureContainer)[(*_pOrdered_Texture_Container)[i]].iTexture_);
			pGLBind_Vertex_Array(iVao_);
            glDrawArrays(GL_TRIANGLES, BASE_INDEX_VERTEX_ARRAY, NUMBER_OF_DROWING_VERTEXES);

            for(int n = 0; n < 4; ++n)
                for(int m = 0; m < 4; ++m)
                {
                    if(n == m)
                    {
                        aMatrix_Model_[n][m] = 64.0f;
                        continue;
                    }

                    aMatrix_Model_[n][m] = 0.0f;
                }
            aMatrix_Model_[3][3] = 1.0f;
		}
	}
 
	void CRenderSystem::SetModelMatrix(Shader* _Shader_Program, const ECS::STransformComponent& _transform_Component)
	{
		aMatrix_Model_[u_iRange-LIMITER][0] = _transform_Component.fPos_X;
		aMatrix_Model_[u_iRange-LIMITER][1] = _transform_Component.fPos_Y;
		aMatrix_Model_[u_iRange-LIMITER][2] = _transform_Component.fPos_Z;
  
		unsigned int uiTransformt_Loc = pGLGet_Uniform_Location(_Shader_Program->iID, "aModel_Matrix");
		pGLUniform_Matrix4fv(uiTransformt_Loc, NUMBER_OF_MATRICES, GL_FALSE, &aMatrix_Model_[0][0]);
	}

	void CRenderSystem::SetProjectionMatrix(Shader* _Shader_Program)
	{
		unsigned int uiTransformt = pGLGet_Uniform_Location(_Shader_Program->iID, "aProjection_Matrix");
		pGLUniform_Matrix4fv(uiTransformt, NUMBER_OF_MATRICES, GL_FALSE, &aMatrix_Ortho_[0][0]);
	}

	void CRenderSystem::SetRotate(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component)
	{
        Matrix<float, 4> trans(1.0f);
        trans = Rotate(trans, Vector<float, 4>(1.0f, 1.0f, 1.0f, 1.0f), _transform_Component.fRotate);
        aMatrix_Model_[u_iRange-LIMITER][0] = _transform_Component.fPos_X;
		aMatrix_Model_[u_iRange-LIMITER][1] = _transform_Component.fPos_Y;
		aMatrix_Model_[u_iRange-LIMITER][2] = _transform_Component.fPos_Z;
//        PrintMatrix(aMatrix_Model_);
//        aMatrix_Model_ = aMatrix_Model_ * trans;

//        PrintMatrix(aMatrix_Model_);
        
//        trans = trans * aMatrix_Model_;

        unsigned int uiTransformt_Loc0 = pGLGet_Uniform_Location(_Shader_Program->iID, "aRotate_Matrix");
		pGLUniform_Matrix4fv(uiTransformt_Loc0, NUMBER_OF_MATRICES, GL_FALSE, &trans[0][0]);
        
        unsigned int uiTransformt_Loc = pGLGet_Uniform_Location(_Shader_Program->iID, "aModel_Matrix");
		pGLUniform_Matrix4fv(uiTransformt_Loc, NUMBER_OF_MATRICES, GL_FALSE, &aMatrix_Model_[0][0]);
//        _transform_Component.fRotate = 0;
	}
    
    void CRenderSystem::SetViewMatrix(Shader* _Shader_Program, const ECS::STransformComponent& _transform_Component)
    {
        aMatrix_View_[0][0] = 1.0;
        aMatrix_View_[1][1] = 1.0;
        aMatrix_View_[2][2] = 1.0;
        aMatrix_View_[3][3] = 1.0;
        aMatrix_View_[u_iRange-LIMITER][0] = -_transform_Component.fPos_X;
        aMatrix_View_[u_iRange-LIMITER][1] = -_transform_Component.fPos_Y;
        // aMatrix_View_[u_iRange-LIMITER][0] = -x_axis;
        // aMatrix_View_[u_iRange-LIMITER][1] = -y_axis;
//        aMatrix_View_[u_iRange-LIMITER][2] = _transform_Component.fPos_Z + 100;
        unsigned int uiTransform_View = pGLGet_Uniform_Location(_Shader_Program->iID, "aView_Matrix");
		pGLUniform_Matrix4fv(uiTransform_View, NUMBER_OF_MATRICES, GL_FALSE, &aMatrix_View_[0][0]);
    }

    void CRenderSystem::PrintMatrix(Matrix<float, 4> _tMatrix)
    {
        for(int i = 0; i < 4; ++i)
            for(int j = 0; j < 4; ++j)
                std::cout << _tMatrix[i][j] << std::endl;
    }
}
