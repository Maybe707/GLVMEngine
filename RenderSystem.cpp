#include "RenderSystem.hpp"
#include "ComponentManager.hpp"
#include "Engine.hpp"
#include "Event.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include "VertexComponent.hpp"
#include "VertexMath.hpp"
#include "ViewComponent.hpp"
#include <GL/gl.h>
#include <cmath>
#include <ctime>
#include <pthread.h>
#include <ratio>

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
		// aMatrix_Ortho_[0]  = 2/1280.0f;
		// //Matrix_Ortho[3]  -= 1;
		// //Matrix_Ortho[7]  -= 1;
		// aMatrix_Ortho_[5]  = 2/1280.0f;
		// aMatrix_Ortho_[10] = 1/(600.0f-0.0f);
		// aMatrix_Ortho_[15] = 1.0f;
		// //Matrix_Ortho[14] = -m_zn/(m_zf-m_zn);

        // tProjection_Matrix[0][0] = 2/1920.0f;
        // tProjection_Matrix[1][1] = 2/1080.0f;
        // tProjection_Matrix[2][2] = 1/(600.0f - 0.0f);
        // tProjection_Matrix[3][3] = 1.0f;

        // float fov = 90;
        // float f = 3, n = 10;
        // float S = std::tan((fov/2)*(PI/360));
        // tProjection_Matrix[0][0] = 1/((1920.0f/1080.0f)*S);
        // tProjection_Matrix[1][1] = 1/S;
        // tProjection_Matrix[2][2] = -((f+n)/(f-n));
        // tProjection_Matrix[2][3] = -((2*f*n)/(f-n));
        // tProjection_Matrix[3][2] = -1;
        // tProjection_Matrix[3][3] = 1;

        // float fov = 60;
        // float f = 3, n = 10;
        // float zm = f - n;
        // float zp = f + n;
        // float S = std::tan((fov/2) * (PI / 360));
        // float fAspect_Ratio = 1920.0f/1080.0f;
        // tProjection_Matrix[0][0] = (1/S) / fAspect_Ratio;
        // tProjection_Matrix[1][1] = 1/S;
        // tProjection_Matrix[2][2] = -((f+n)/(f-n));
        // tProjection_Matrix[2][3] = -((2*f*n)/(f-n));
        // tProjection_Matrix[3][2] = -1;
        // tProjection_Matrix[3][3] = 1;

        tProjection_Matrix[0][0] = 1;
        tProjection_Matrix[1][1] = 1;
        tProjection_Matrix[2][2] = 1;
        tProjection_Matrix[3][3] = 1;

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
		glViewport(0, 0, 1920, 1080);
	}

	CRenderSystem::~CRenderSystem()
	{
		pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &iVao_);
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &iVbo_);
	}

	void CRenderSystem::Update(CComponentManager& _Component_Manager, Core::CEvent& _Event)
	{
		Core::TCVectorContainer<STransformComponent>* _tTransformContainer = GetInnerComponentContainer<STransformComponent>(_Component_Manager);
		Core::TCVectorContainer<CTextureComponent>* _tTextureContainer = GetInnerComponentContainer<CTextureComponent>(_Component_Manager);
		Core::TCVectorContainer<SVertexComponent>* _pVertex_Container = GetInnerComponentContainer<SVertexComponent>(_Component_Manager);
        Core::TCVectorContainer<CViewComponent>* _pView_Container = GetInnerComponentContainer<CViewComponent>(_Component_Manager);
        Core::TCVectorContainer<unsigned int>* _pOrdered_View_Container = GetInnerIDsContainer<CViewComponent>(_Component_Manager);
		Core::TCVectorContainer<unsigned int>* _pOrdered_Texture_Container = GetInnerIDsContainer<CTextureComponent>(_Component_Manager);
        ECS::STransformComponent* Player;
        for(int j = 0, iSize = _pOrdered_View_Container->GetSize(); j < iSize; ++j)
        {
            Player = &(*_tTransformContainer)[(*_pOrdered_View_Container)[j]];
            SetViewMatrix(_Shader_Program, (*_tTransformContainer)[(*_pOrdered_View_Container)[j]], _Event, *Player);
        }
		for(int i = 0, iSize = _pOrdered_Texture_Container->GetSize(); i < iSize; ++i)
		{
            //(*_tTransformContainer)[(*_pOrdered_Texture_Container)[0]].fRotate += 1;
			pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof((*_pVertex_Container)[(*_pOrdered_Texture_Container)[i]].aVertex_), &(*_pVertex_Container)[(*_pOrdered_Texture_Container)[i]].aVertex_, GL_DYNAMIC_DRAW);

            SetModelMatrix(_Shader_Program, (*_tTransformContainer)[(*_pOrdered_Texture_Container)[i]], *Player);
            
			pGLActive_Texture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, (*_tTextureContainer)[(*_pOrdered_Texture_Container)[i]].iTexture_);
			pGLBind_Vertex_Array(iVao_);
            glDrawArrays(GL_TRIANGLES, BASE_INDEX_VERTEX_ARRAY, NUMBER_OF_DROWING_VERTEXES);
		}
	}

	void CRenderSystem::SetProjectionMatrix(Shader* _Shader_Program)
	{
		unsigned int uiTransformt = pGLGet_Uniform_Location(_Shader_Program->iID, "aProjection_Matrix");
		pGLUniform_Matrix4fv(uiTransformt, NUMBER_OF_MATRICES, GL_FALSE, &tProjection_Matrix[0][0]);
	}

	void CRenderSystem::SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component, ECS::STransformComponent& _Player)
	{
        Matrix<float, 4> tRotation_Matrix(1.0f);
        Matrix<float, 4> tModel_Matrix(1.0f);
        Matrix<float, 4> tScaling_Matrix(1.0f);
        Matrix<float, 4> tTranslation_Matrix(1.0f);
        
        // tRotation_Matrix = Rotate(tRotation_Matrix, Vector<float, 4>(0.0f, 1.0f, 0.0f, 1.0f), _Event.mouse_Pointer_Position_.u_iX);
        // tRotation_Matrix = Rotate(tRotation_Matrix, Vector<float, 4>(1.0f, 0.0f, 0.0f, 1.0f), _Event.mouse_Pointer_Position_.u_iY);

        // float radius = 10.0f
        // float PI = 3.14f;
        // float fAngle = _Event.mouse_Pointer_Position_.u_iX / 180;
        // fAngle = fAngle * (PI / 180);
                
        // float cam_x = std::sin(fAngle) * radius;

        // tTranslation_Matrix = LookAtRH(Vector<float, 3>(_Player.fPos_X, _Player.fPos_Y, _Player.fPos_Z),
        //                                Vector<float, 3>(_transform_Component.fPos_X, _transform_Component.fPos_Y, _transform_Component.fPos_Z),
        //                                Vector<float, 3>(0.0f, 1.0f, 0.0f));
        
        // tTranslation_Matrix[u_iRange-LIMITER][0] = _transform_Component.fPos_X;
		// tTranslation_Matrix[u_iRange-LIMITER][1] = _transform_Component.fPos_Y;
		// tTranslation_Matrix[u_iRange-LIMITER][2] = _transform_Component.fPos_Z;
		// tTranslation_Matrix[u_iRange-LIMITER][3] = 1.0f;

        tScaling_Matrix[0][0] = _transform_Component.fScale;
        tScaling_Matrix[1][1] = _transform_Component.fScale;
        tScaling_Matrix[2][2] = _transform_Component.fScale;
		tScaling_Matrix[3][3] = 1.0f;
        
        /// For normal matrices.
        
        tTranslation_Matrix[3][0] = _transform_Component.fPos_X;
		tTranslation_Matrix[3][1] = _transform_Component.fPos_Y;
		tTranslation_Matrix[3][2] = _transform_Component.fPos_Z;
        // tModel_Matrix.SelfTensorTranspose();
        
        tModel_Matrix = tScaling_Matrix * tTranslation_Matrix;
//		tModel_Matrix = tTranslation_Matrix * tScaling_Matrix;

		PrintMatrix(tModel_Matrix);

        unsigned int uiTransformt_Loc = pGLGet_Uniform_Location(_Shader_Program->iID, "aModel_Matrix");
		pGLUniform_Matrix4fv(uiTransformt_Loc, NUMBER_OF_MATRICES, GL_FALSE, &tModel_Matrix[0][0]);
	}
    
    void CRenderSystem::SetViewMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component, Core::CEvent& _Event,
                                      ECS::STransformComponent& _Player)
    {
        Matrix<float, 4> tView_Matrix(1.0f);
        
        // tView_Matrix[u_iRange-LIMITER][0] = -_transform_Component.fPos_X;
        // tView_Matrix[u_iRange-LIMITER][1] = -_transform_Component.fPos_Y;

//        aMatrix_View_[u_iRange-LIMITER][2] = _transform_Component.fPos_Z + 100;

        // tView_Matrix[u_iRange-LIMITER][0] = -(_Event.mouse_Pointer_Position_.u_iX / 2.0f);
        // tView_Matrix[u_iRange-LIMITER][1] = _Event.mouse_Pointer_Position_.u_iY;

        // tView_Matrix[u_iRange-LIMITER][0] = -(_Event.mouse_Pointer_Position_.u_iX - 960.0f);
        // tView_Matrix[u_iRange-LIMITER][1] = (_Event.mouse_Pointer_Position_.u_iY - 540.0f);

        /*!
        tView_Matrix[u_iRange-LIMITER][0] = -(_Event.mouse_Pointer_Position_.u_iX - 960.0f) -_transform_Component.fPos_X;
        tView_Matrix[u_iRange-LIMITER][1] = (_Event.mouse_Pointer_Position_.u_iY - 540.0f) - _transform_Component.fPos_Y;
        tView_Matrix[u_iRange-LIMITER][2] = -_transform_Component.fPos_Z + 100;
        */

        // tView_Matrix[u_iRange-LIMITER][0] = 0;
        // tView_Matrix[u_iRange-LIMITER][1] = 0;

        // tView_Matrix[u_iRange-LIMITER][0] = -_transform_Component.fPos_X;
        // tView_Matrix[u_iRange-LIMITER][2] = -_transform_Component.fPos_Z + 100;

        const float radious = 10.0f;
        // tView_Matrix = LookAtRH(Vector<float, 3>(_Event.mouse_Pointer_Position_.u_iX-960, 0, _Event.mouse_Pointer_Position_.u_iY-540),
        //                         Vector<float, 3>(_transform_Component.fPos_X, _transform_Component.fPos_Y, _transform_Component.fPos_Z),
        //                         {0, 1.0f, 0});

        // tView_Matrix = LookAtRH(Vector<float, 3>(0.3f, 0.2f, 0.5f),
        //                         Vector<float, 3>(_transform_Component.fPos_X, _transform_Component.fPos_Y, _transform_Component.fPos_Z),
        //                         {0, 1.0f, 0});

        // tView_Matrix = FPS_View_RH(Vector<float, 3>(_transform_Component.fPos_X, _transform_Component.fPos_Y,
        //                                _transform_Component.fPos_Z),
        //                            (_Event.mouse_Pointer_Position_.u_iY / 6.0f - 90.0f),
        //                            (_Event.mouse_Pointer_Position_.u_iX / 3.0f));

        // tView_Matrix[3][0] = _transform_Component.fPos_X;
        // tView_Matrix[3][1] = _transform_Component.fPos_Y;
        // tView_Matrix[3][2] = _transform_Component.fPos_Z;

        tView_Matrix = LookAtRH(Vector<float, 3>(_Player.fPos_X, _Player.fPos_Y, _Player.fPos_Z),
                                Vector<float, 3>(0.0f, 0.0f, 0.5f),
                                Vector<float, 3>(0.0f, 1.0f, 0.0f));
        
        std::cout << "View matrix x: " << tView_Matrix[3][0] << std::endl;
        std::cout << "View matrix y: " << tView_Matrix[3][1] << std::endl;
        std::cout << "View matrix y: " << tView_Matrix[3][2] << std::endl;
		
        unsigned int uiTransform_View = pGLGet_Uniform_Location(_Shader_Program->iID, "aView_Matrix");
		pGLUniform_Matrix4fv(uiTransform_View, NUMBER_OF_MATRICES, GL_FALSE, &tView_Matrix[0][0]);
    }

    void CRenderSystem::PrintMatrix(Matrix<float, 4> _tMatrix)
    {
        for(int i = 0; i < 4; ++i)
            for(int j = 0; j < 4; ++j)
                std::cout << _tMatrix[i][j] << std::endl;
    }
}
