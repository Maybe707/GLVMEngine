#include "GraphicAPI/Opengl.hpp"
#include "ComponentManager.hpp"
#include "Components/LightComponent.hpp"
#include "Components/MaterialComponent.hpp"
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
#include "WavefrontObjParser.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ratio>
#include <thread>

namespace GLVM::Core
{
    COpenglRenderer::COpenglRenderer()
	{
		_Shader_Program     = new Shader("../GLshaders/Shader.vs", "../GLshaders/Shader.fs");

        glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, 1920, 1080);
	}

	COpenglRenderer::~COpenglRenderer()
	{
        delete _Shader_Program;
        _Shader_Program = nullptr;

		for (int i = 0; i < VAOcontainer_.size(); ++i)
			pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &VAOcontainer_[i]);
		
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &iVbo_);
	}
    
	void COpenglRenderer::draw()
	{
        ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
        // Core::TCVectorContainer<unsigned int>* pEntity_Container_refTexture =
        //     ECS::GetInnerIDsContainer<ECS::CTextureComponent>(*pComponent_Manager);
        // unsigned int uiVector_Texture_Size = pEntity_Container_refTexture->GetSize();
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefView = ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
		unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		ECS::CViewComponent& playerViewComponent = pComponent_Manager->GetComponent<ECS::CViewComponent>(uiPlayerEntity);

        _Shader_Program->Use();
//		_Shader_Program->SetUniformID("tex", 10);
		_Shader_Program->SetUniformID("material.diffuse", 10);
		_Shader_Program->SetUniformID("material.specular", 11);

		// auto start = std::chrono::system_clock::now();
		// auto time_now = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch());
		// double current_time = time_now.count();
		// current_time /= 1000.0f;
		// std::cout << "time: " << current_time  << std::endl;
		// vec3 lightColor;
		// lightColor[0] = std::sin(current_time * 2.0f);
		// lightColor[1] = std::sin(current_time * 0.7f);
		// lightColor[2] = std::sin(current_time * 1.3f);

		// vec3 diffuseColor = lightColor * vec3(0.5f);
		// vec3 ambientColor = diffuseColor * vec3(0.2f);
		
//		_Shader_Program->SetVec3("light.position", 1.5f, 1.5f, 2.9f);
		_Shader_Program->SetVec3("viewPosition", playerViewComponent.Position[0],
								 playerViewComponent.Position[1],
								 playerViewComponent.Position[2]);

		// Chrome material.
		// _Shader_Program->SetVec3("material.ambient", 0.25f, 0.25f, 0.25f);
		// _Shader_Program->SetVec3("material.diffuse", 0.4f, 0.4f, 0.4f);
		// _Shader_Program->SetVec3("material.specular", 0.774597f, 0.774597f, 0.774597f);
		// _Shader_Program->SetFloat("material.shininess", 128.0f * 0.6f);

		// !!!!!!!!!!!!!!!!!!!!!!
		// Core::TCVectorContainer<unsigned int>* pEntityContainerRefMaterial = ECS::GetInnerIDsContainer<Core::SMaterialComponent>(*pComponent_Manager);
		// unsigned int MaterialComponentContainerSize = pEntityContainerRefMaterial->GetSize();
		// for(int v = 0; v < MaterialComponentContainerSize; ++v) {
		// 	unsigned int uiMaterialEntity = (*pEntityContainerRefMaterial)[v];
		// 	Core::SMaterialComponent& materialComponent = pComponent_Manager->GetComponent<Core::SMaterialComponent>(uiMaterialEntity);
		// 	_Shader_Program->SetFloat("material.shininess", materialComponent.shininess);
		// 	_Shader_Program->SetVec3("material.ambient",  materialComponent.ambient[0], materialComponent.ambient[1], materialComponent.ambient[2]);
		// 	_Shader_Program->SetVec3("material.diffuse",  materialComponent.diffuse[0], materialComponent.diffuse[1], materialComponent.diffuse[2]); // darken diffuse light a bit
		// 	_Shader_Program->SetVec3("material.specular", materialComponent.specular[0], materialComponent.specular[1], materialComponent.specular[2]);
		// }

		// Black rubber material.
		// _Shader_Program->SetVec3("material.ambient", 0.02f, 0.02f, 0.02f);
		// _Shader_Program->SetVec3("material.diffuse", 0.01f, 0.01f, 0.01f);
		// _Shader_Program->SetVec3("material.specular", 0.4f, 0.4f, 0.4f);
		// _Shader_Program->SetFloat("material.shininess", 128.0f * 0.078125f);

		// _Shader_Program->SetVec3("light.ambient",  ambientColor[0], ambientColor[1], ambientColor[2]);
		// _Shader_Program->SetVec3("light.diffuse",  diffuseColor[0], diffuseColor[1], diffuseColor[2]); // darken diffuse light a bit
		// _Shader_Program->SetVec3("light.ambient",  0.2f, 0.2f, 0.2f);
		// _Shader_Program->SetVec3("light.diffuse",  0.5f, 0.5f, 0.5f); // darken diffuse light a bit
		// _Shader_Program->SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefLight = ECS::GetInnerIDsContainer<Core::SLightComponent>(*pComponent_Manager);
		unsigned int LightComponentContainerSize = pEntityContainerRefLight->GetSize();
		for(int x = 0; x < LightComponentContainerSize; ++x) {
			unsigned int uiLightEntity = (*pEntityContainerRefLight)[x];
			Core::SLightComponent& lightComponent = pComponent_Manager->GetComponent<Core::SLightComponent>(uiLightEntity);
			_Shader_Program->SetVec3("light.position", lightComponent.position[0], lightComponent.position[1], lightComponent.position[2]);
			_Shader_Program->SetVec3("light.ambient",  lightComponent.ambient[0], lightComponent.ambient[1], lightComponent.ambient[2]);
			_Shader_Program->SetVec3("light.diffuse",  lightComponent.diffuse[0], lightComponent.diffuse[1], lightComponent.diffuse[2]); // darken diffuse light a bit
			_Shader_Program->SetVec3("light.specular", lightComponent.specular[0], lightComponent.specular[1], lightComponent.specular[2]);
		}
		
		for(int i = 0; i < texture_load_data_.size(); ++i)
			for (int j = 0; j < texture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = texture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).vkVertexId_;
				unsigned int textureID = pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture).id_;
				LoadTextureData(texture_load_data_[textureID]);
//				LoadTextureData(texture_load_data_[i]);
				SetModelMatrix(_Shader_Program, pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				pGLActive_Texture(GL_TEXTURE10);
				pGLActive_Texture(GL_TEXTURE11);
				pGLBind_Vertex_Array(VAOcontainer_[uiVertexId]);
				Core::SMaterialComponent& materialComponent = pComponent_Manager->GetComponent<Core::SMaterialComponent>(uiEntity_refTexture);
				_Shader_Program->SetFloat("material.shininess", materialComponent.shininess);
				_Shader_Program->SetVec3("material.ambient",  materialComponent.ambient[0], materialComponent.ambient[1], materialComponent.ambient[2]);
				// _Shader_Program->SetVec3("material.diffuse",  materialComponent.diffuse[0], materialComponent.diffuse[1], materialComponent.diffuse[2]); // darken diffuse light a bit
				_Shader_Program->SetVec3("material.specular", materialComponent.specular[0], materialComponent.specular[1], materialComponent.specular[2]);
				
				glDrawElements(GL_TRIANGLES, aIndices_[uiVertexId].size(), GL_UNSIGNED_INT, 0);
			}

		for(int i = 0; i < hudTexture_load_data_.size(); ++i)
			for (int j = 0; j < hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).vkVertexId_;
				LoadTextureData(hudTexture_load_data_[pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture).id_]);
				SetModelMatrix(_Shader_Program, pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				pGLActive_Texture(GL_TEXTURE10);
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
        
        pGLVertex_Attrib_Pointer(LAYOUT_0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)VERTEX_OFFSET);
        pGLEnable_Vertex_Attrib_Array(LAYOUT_0);
		pGLVertex_Attrib_Pointer(LAYOUT_1, TEXTURE_SIZE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(TEXTURE_OFFSET * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(LAYOUT_1);
		pGLVertex_Attrib_Pointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(2);

		VBOcontainer_.push_back(iVbo_);
		VAOcontainer_.push_back(iVao_);
		EBOcontainer_.push_back(iEbo_);
	}
	
    void COpenglRenderer::loadWavefrontObj() {
        for (int m = 0; m < pathsArray_.size(); ++m) {
            CWaveFrontObjParser parser;
            CWaveFrontObjParser* wavefrontObjParser = &parser;

            wavefrontObjParser->ReadFile(pathsArray_[m]);
			std::cout << "Read" << std::endl;
            wavefrontObjParser->ParseFile();
			std::cout << "Parse" << std::endl;
			aVertexes_.emplace_back();
            aIndices_.emplace_back();
            
            unsigned int vertexIndex = 0;
            unsigned int textureIndex = 0;
			unsigned int normalIndex = 0;
            unsigned int faceVerticesSize = wavefrontObjParser->getFaces().GetSize();
            for (int i = 0; i < faceVerticesSize; ++i)
                for (int j = 0; j < 3; ++j) {
                    vertexIndex = wavefrontObjParser->getFaces()[i][0][j] - 1;
                    aIndices_[m].push_back(i * 3 + j);
                    SVertex vertex = wavefrontObjParser->getCoordinateVertices()[vertexIndex];
                    textureIndex = wavefrontObjParser->getFaces()[i][1][j] - 1;
                    SVertex texture = wavefrontObjParser->getTextureVertices()[textureIndex];
					normalIndex = wavefrontObjParser->getFaces()[i][2][j] - 1;
					SVertex normal = wavefrontObjParser->getNormals()[normalIndex];
					aVertexes_[m].push_back(vertex[0]);
					aVertexes_[m].push_back(vertex[1]);
					aVertexes_[m].push_back(vertex[2]);
					aVertexes_[m].push_back(texture[0]);
					aVertexes_[m].push_back(texture[1]);
					aVertexes_[m].push_back(normal[0]);
					aVertexes_[m].push_back(normal[1]);
					aVertexes_[m].push_back(normal[2]);
                }
			SetVertices(aIndices_[m], aVertexes_[m]);
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
        unsigned int uniformLocationViewWorld = pGLGet_Uniform_Location(_Shader_Program->iID, "aView_Matrix");
        pGLUniform_Matrix4fv(uniformLocationViewWorld, NUMBER_OF_MATRICES, GL_FALSE, &_viewMatrix[0][0]);
    }

    void COpenglRenderer::SetProjectionMatrix(mat4 _projectionMatrix) {
        unsigned int uniformLocationProjectionWorld = pGLGet_Uniform_Location(_Shader_Program->iID, "aProjection_Matrix");
		pGLUniform_Matrix4fv(uniformLocationProjectionWorld, NUMBER_OF_MATRICES, GL_FALSE, &_projectionMatrix[0][0]);
    }
    
    void COpenglRenderer::SetTextureData(std::vector<ECS::CTexture>& _texture_data, std::vector<ECS::CTexture>& _hud_texture_data) {
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
