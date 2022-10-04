#include "GraphicAPI/Opengl.hpp"
#include "ComponentManager.hpp"
#include "Constants.hpp"
#include "Engine.hpp"
#include "Event.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "GLPointer.h"
#include "MeshManager.hpp"
#include "Texture.hpp"
#include "VectorContainer.hpp"
#include "Components/VertexComponent.hpp"
#include "VertexData.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include <GL/gl.h>
#include <cmath>
#include "Globals.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <thread>

namespace GLVM::Core
{
    COpenglRenderer::COpenglRenderer()
	{
		_Shader_Program     = new Shader("../Shader.vs", "../Shader.fs");
        GUI_Shader_Program_ = new Shader("../GUIShader.vs", "../GUIShader.fs");

        glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, 1920, 1080);
	}

	COpenglRenderer::~COpenglRenderer()
	{
        delete _Shader_Program;
        _Shader_Program = nullptr;
        delete GUI_Shader_Program_;
        GUI_Shader_Program_ = nullptr;

		for (int i = 0; i < VAOcontainer_.size(); ++i)
			pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &VAOcontainer_[i]);
		
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
        
		for(int i = 0; i < texture_load_data_.size(); ++i)
			for (int j = 0; j < texture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = texture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).vkVertexId_;
//				SetVertices(aIndices_[uiVertexId], aVertices_[uiVertexId]);
				LoadTextureData(texture_load_data_[pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture).id_]);
				SetModelMatrix(_Shader_Program, pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				pGLActive_Texture(GL_TEXTURE10);
//				glBindTexture(GL_TEXTURE_2D, texture_load_data_[pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture).id_].iTexture_);
				pGLBind_Vertex_Array(VAOcontainer_[uiVertexId]);
				glDrawElements(GL_TRIANGLES, aIndices_[uiVertexId].size(), GL_UNSIGNED_INT, 0);
			}

		for(int i = 0; i < hudTexture_load_data_.size(); ++i)
			for (int j = 0; j < hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).vkVertexId_;
//				SetVertices(aIndices_[uiVertexId], aVertices_[uiVertexId]);
				LoadTextureData(hudTexture_load_data_[pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture).id_]);
				SetModelMatrix(_Shader_Program, pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				pGLActive_Texture(GL_TEXTURE10);
//				glBindTexture(GL_TEXTURE_2D, texture_load_data_[pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture).id_].iTexture_);
				pGLBind_Vertex_Array(VAOcontainer_[uiVertexId]);
				glDrawElements(GL_TRIANGLES, aIndices_[uiVertexId].size(), GL_UNSIGNED_INT, 0);
			}
	}

	void COpenglRenderer::SetVertices(std::vector<unsigned int>& _aIndices,
									  std::vector<float>& _aVertices) {
		GLuint iVbo_;
		GLuint iVao_;
		GLuint iEbo_;
		pGLGen_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &iVao_);
        pGLGen_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &iVbo_);
  
        pGLGen_Buffers(1, &iEbo_);
        
        ///< First we link the vertex array object, then we link and set the vertex buffers, and then we configure the vertex attributes.
        
        pGLBind_Vertex_Array(iVao_);
		
		pGLBind_Buffer(GL_ARRAY_BUFFER, iVbo_);
        pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(float) * _aVertices.size(), _aVertices.data(), GL_DYNAMIC_DRAW);

        pGLBind_Buffer(GL_ELEMENT_ARRAY_BUFFER, iEbo_);
        pGLBuffer_Data(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _aIndices.size(), _aIndices.data(), GL_STATIC_DRAW);
        
        pGLVertex_Attrib_Pointer(LAYOUT_0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, SIZE_OF_VERTEX_DATA * sizeof(float), (void*)VERTEX_OFFSET);
        pGLEnable_Vertex_Attrib_Array(LAYOUT_0);
		pGLVertex_Attrib_Pointer(LAYOUT_1, TEXTURE_SIZE, GL_FLOAT, GL_FALSE, SIZE_OF_VERTEX_DATA * sizeof(float), (void*)(TEXTURE_OFFSET * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(LAYOUT_1);
		VAOcontainer_.push_back(iVbo_);
		VAOcontainer_.push_back(iVao_);
		EBOcontainer_.push_back(iEbo_);
	}
	
    void COpenglRenderer::loadWavefrontObj() {
        for (int m = 0; m < pathsArray_.size(); ++m) {
            CWaveFrontObjParser parser;
            CWaveFrontObjParser* wavefrontObjParser = &parser;

            wavefrontObjParser->ReadFile(pathsArray_[m]);
            wavefrontObjParser->ParseFile();

			std::vector<float> aVertexes;
            aIndices_.emplace_back();
//            aVertices_.emplace_back();
            
            unsigned int vertexIndex = 0;
            unsigned int textureIndex = 0;
            unsigned int faceVerticesSize = wavefrontObjParser->getFaces().GetSize();
            for (int i = 0; i < faceVerticesSize; ++i)
                for (int j = 0; j < 3; ++j) {
                    vertexIndex = wavefrontObjParser->getFaces()[i][0][j] - 1;
                    aIndices_[m].push_back(i * 3 + j);
                    SVertex vertex = wavefrontObjParser->getCoordinateVertices()[vertexIndex];
                    textureIndex = wavefrontObjParser->getFaces()[i][1][j] - 1;
                    SVertex texture = wavefrontObjParser->getTextureVertices()[textureIndex];
                    // aVertices_[m].push_back({{vertex[0], vertex[1], vertex[2]}, {0.0f, 0.0f, 0.0f}, {texture[0], texture[1]}});
					// std::cout << "v0: " << vertex[0] << " v1: " << vertex[1] << " v2: " << vertex[2]
					// 		  << " t0: " << texture[0] << " t1: " << texture[1] << std::endl;
					aVertexes.push_back(vertex[0]);
					aVertexes.push_back(vertex[1]);
					aVertexes.push_back(vertex[2]);
					aVertexes.push_back(texture[0]);
					aVertexes.push_back(texture[1]);
                }
			SetVertices(aIndices_[m], aVertexes);
        }
    }
    
 	void COpenglRenderer::LoadTextureData(GLVM::ECS::CTexture& _Texture)
	{
		///< Loading and creating texture.
		glGenTextures(NUMBER_OF_CREATING_TEXTURE_OBJECT_1, &_Texture.iTexture_);
		glBindTexture(GL_TEXTURE_2D, _Texture.iTexture_);
	
		///< Setting texture applying parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		///< Loading image, creating texture and generation mipmap-levels
		glTexImage2D(GL_TEXTURE_2D, MIPMAP_LEVEL, GL_RGBA, _Texture.iWidth_, _Texture.iHeight_, SOME_OLD_STUFF, GL_RGBA, GL_UNSIGNED_BYTE, _Texture.u_iData_);
		pGLGenerate_Mipmap(GL_TEXTURE_2D);

		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    void COpenglRenderer::SetViewMatrix(mat4 _viewMatrix) {
        _Shader_Program->Use();
        _Shader_Program->SetUniformID();
        
        unsigned int uiTransform_View = pGLGet_Uniform_Location(_Shader_Program->iID, "aView_Matrix");
        pGLUniform_Matrix4fv(uiTransform_View, NUMBER_OF_MATRICES, GL_FALSE, &_viewMatrix[0][0]);
    }

    void COpenglRenderer::SetProjectionMatrix(mat4 _projectionMatrix) {
        unsigned int uiTransformt = pGLGet_Uniform_Location(_Shader_Program->iID, "aProjection_Matrix");
		pGLUniform_Matrix4fv(uiTransformt, NUMBER_OF_MATRICES, GL_FALSE, &_projectionMatrix[0][0]);
    }
    
    void COpenglRenderer::SetTextureData(std::vector<ECS::CTexture> _texture_data, std::vector<ECS::CTexture> _hud_texture_data) {
		texture_load_data_ = _texture_data;
		hudTexture_load_data_ = _hud_texture_data;
	}
	
    void COpenglRenderer::SetMeshData(std::vector<const char*> _pathsArray) {
		for (int i = 0; i < _pathsArray.size(); ++i)
            pathsArray_.push_back(_pathsArray[i]);
	}
    
    void COpenglRenderer::run() {
		loadWavefrontObj();
	}
}
