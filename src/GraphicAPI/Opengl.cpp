#include "GraphicAPI/Opengl.hpp"
#include "ComponentManager.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Constants.hpp"
#include "Engine.hpp"
#include "Event.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "GLPointer.h"
#include "MeshManager.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "ToString.hpp"
#include "VectorContainer.hpp"
#include "Components/VertexComponent.hpp"
#include "VertexData.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include <GL/gl.h>
#include <cmath>
#include "Globals.hpp"
#include "WavefrontObjParser.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ratio>
#include <thread>

namespace GLVM::Core
{
    COpenglRenderer::COpenglRenderer()
	{
		_Shader_Program     = new Shader("../GLshaders/Shader.vert", "../GLshaders/Shader.frag");
		shadowMappingDepth_ = new Shader("../GLshaders/ShadowMappingDepth.vert", "../GLshaders/ShadowMappingDepth.frag");
		debugQuadDepth_     = new Shader("../GLshaders/DebugQuadDepth.vert", "../GLshaders/DebugQuadDepth.frag");

		pGLGen_Framebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthMapTexture);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		pGLBind_Framebuffer(GL_FRAMEBUFFER, depthMapFBO);
		pGLFramebuffer_Texture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);
		
        glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	COpenglRenderer::~COpenglRenderer()
	{
        delete _Shader_Program;
        _Shader_Program = nullptr;

		for (int i = 0; i < VBOcontainer_.size(); ++i)
			pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &VBOcontainer_[i]);
		
		for (int i = 0; i < VAOcontainer_.size(); ++i)
			pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &VAOcontainer_[i]);
		
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &quadVBO_);
		pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &quadVAO_);
	}
    
	void COpenglRenderer::draw()
	{
        ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefView = ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
		unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		ECS::CViewComponent& playerViewComponent = pComponent_Manager->GetComponent<ECS::CViewComponent>(uiPlayerEntity);
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefDirectionalLight = ECS::GetInnerIDsContainer<Core::SDirectionalLightComponent>(*pComponent_Manager);
		unsigned int directionalLightComponentContainerSize = pEntityContainerRefDirectionalLight->GetSize();

		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float planeVertices[] = {
			// positions            // normals         // texcoords
			25.0f, 0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, 0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-25.0f, 0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

			25.0f, 0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, 0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			25.0f, 0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};
		// plane VAO
		pGLGen_Vertex_Arrays(1, &planeVAO_);
		pGLGen_Buffers(1, &planeVBO_);
		pGLBind_Vertex_Array(planeVAO_);
		pGLBind_Buffer(GL_ARRAY_BUFFER, planeVBO_);
		pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		pGLEnable_Vertex_Attrib_Array(0);
		pGLVertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		pGLEnable_Vertex_Attrib_Array(1);
		pGLVertex_Attrib_Pointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(2);
		pGLVertex_Attrib_Pointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		pGLBind_Vertex_Array(0);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Matrix<float, 4> lightProjection(1.0f), lightView(1.0f), lightSpaceMatrix(1.0f);
		float nearPlane = 1.0f, farPlane = 17.5f;
//		vec3 lightPosition = { 3.0f, 5.0f, 1.0f };
		vec3 lightPosition = playerViewComponent.Position;
		vec3 directionVector = { 0.0f, 0.0f, 0.0f };
		vec3 lightUpVector = { 0.0f, 1.0f, 0.0f };
		for(int x = 0; x < directionalLightComponentContainerSize; ++x) {
			unsigned int uiDirectionalLightEntity = (*pEntityContainerRefDirectionalLight)[x];
			Core::SDirectionalLightComponent& directionalLightComponent = pComponent_Manager->GetComponent<Core::SDirectionalLightComponent>(uiDirectionalLightEntity);
			lightProjection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
			lightView = LookAtMain(lightPosition, directionVector, lightUpVector);
			lightSpaceMatrix = lightView * lightProjection;
			// Render scene from light's point of view
			shadowMappingDepth_->Use();
			unsigned int uiTransformt_Loc = pGLGet_Uniform_Location(shadowMappingDepth_->iID, "lightSpaceMatrix");
			pGLUniform_Matrix4fv(uiTransformt_Loc, NUMBER_OF_MATRICES, GL_FALSE, &lightSpaceMatrix[0][0]);
			// Render to depth map
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			pGLBind_Framebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			// Matrix<float, 4> model(1.0f);
			// unsigned int uiTransformt_Loc_Model = pGLGet_Uniform_Location(shadowMappingDepth_->iID, "aModel_Matrix");
			// pGLUniform_Matrix4fv(uiTransformt_Loc_Model, NUMBER_OF_MATRICES, GL_FALSE, &model[0][0]);
			// glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D, 1);
			// pGLBind_Vertex_Array(planeVAO_);
			// glDrawArrays(GL_TRIANGLES, 0, 6);
			RenderScene(shadowMappingDepth_);
			pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);
		}

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        _Shader_Program->Use();
		_Shader_Program->SetUniformID("material.diffuse", 5);
		_Shader_Program->SetUniformID("material.specular", 6);
		_Shader_Program->SetUniformID("shadowMap", 7);
		_Shader_Program->SetUniformID("diffuseTexture", 8);
		_Shader_Program->SetVec3("viewPosition", playerViewComponent.Position[0],
								 playerViewComponent.Position[1],
								 playerViewComponent.Position[2]);
		
		// _Shader_Program->SetInt("directionalLightsArraySize", directionalLightComponentContainerSize);
		// for(int x = 0; x < directionalLightComponentContainerSize; ++x) {
		// 	unsigned int uiDirectionalLightEntity = (*pEntityContainerRefDirectionalLight)[x];
		// 	Core::SDirectionalLightComponent& directionalLightComponent = pComponent_Manager->GetComponent<Core::SDirectionalLightComponent>(uiDirectionalLightEntity);
		// 	std::string leftString = "directionalLights[";
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].direction"), directionalLightComponent.direction[0], directionalLightComponent.direction[1], directionalLightComponent.direction[2]);

		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),  directionalLightComponent.ambient[0], directionalLightComponent.ambient[1], directionalLightComponent.ambient[2]);
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),  directionalLightComponent.diffuse[0], directionalLightComponent.diffuse[1], directionalLightComponent.diffuse[2]); // darken diffuse light a bit
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"), directionalLightComponent.specular[0], directionalLightComponent.specular[1], directionalLightComponent.specular[2]);
		// }

		// Core::TCVectorContainer<unsigned int>* pEntityContainerRefPointLight = ECS::GetInnerIDsContainer<Core::SPointLightComponent>(*pComponent_Manager);
		// unsigned int pointLightComponentContainerSize = pEntityContainerRefPointLight->GetSize();
		// _Shader_Program->SetInt("pointLightsArraySize", pointLightComponentContainerSize);
		// for(int x = 0; x < pointLightComponentContainerSize; ++x) {
		// 	unsigned int uiPointLightEntity = (*pEntityContainerRefPointLight)[x];
		// 	Core::SPointLightComponent& pointLightComponent = pComponent_Manager->GetComponent<Core::SPointLightComponent>(uiPointLightEntity);
		// 	std::string leftString = "pointLights[";
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
		// 							 pointLightComponent.position[0],
		// 							 pointLightComponent.position[1],
		// 							 pointLightComponent.position[2]);
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
		// 							 pointLightComponent.ambient[0],
		// 							 pointLightComponent.ambient[1],
		// 							 pointLightComponent.ambient[2]);
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
		// 							 pointLightComponent.diffuse[0],
		// 							 pointLightComponent.diffuse[1],
		// 							 pointLightComponent.diffuse[2]); // darken diffuse light a bit
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
		// 							 pointLightComponent.specular[0],
		// 							 pointLightComponent.specular[1],
		// 							 pointLightComponent.specular[2]);
		// 	_Shader_Program->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].constant"),
		// 							  pointLightComponent.constant);
		// 	_Shader_Program->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].linear"),
		// 							  pointLightComponent.linear);
		// 	_Shader_Program->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].quadratic"),
		// 							  pointLightComponent.quadratic);
		// }

		// Core::TCVectorContainer<unsigned int>* pEntityContainerRefSpotLight = ECS::GetInnerIDsContainer<ECS::SSpotLightComponent>(*pComponent_Manager);
		// unsigned int spotLightComponentContainerSize = pEntityContainerRefSpotLight->GetSize();
		// _Shader_Program->SetInt("spotLightsArraySize", spotLightComponentContainerSize);
		// for(int x = 0; x < spotLightComponentContainerSize; ++x) {
		// 	unsigned int uiSpotLightEntity = (*pEntityContainerRefSpotLight)[x];
		// 	ECS::SSpotLightComponent& spotLightComponent = pComponent_Manager->GetComponent<ECS::SSpotLightComponent>(uiSpotLightEntity);
		// 	std::string leftString = "spotLights[";
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
		// 							 spotLightComponent.position[0],
		// 							 spotLightComponent.position[1],
		// 							 spotLightComponent.position[2]);
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].direction"),
		// 							 spotLightComponent.direction[0],
		// 							 spotLightComponent.direction[1],
		// 							 spotLightComponent.direction[2]);
		// 	_Shader_Program->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].cutOff"),
		// 							  std::cos(Radians(spotLightComponent.cutOff)));
		// 	_Shader_Program->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].outerCutOff"),
		// 							  std::cos(Radians(spotLightComponent.outerCutOff)));
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
		// 							 spotLightComponent.ambient[0],
		// 							 spotLightComponent.ambient[1],
		// 							 spotLightComponent.ambient[2]);
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
		// 							 spotLightComponent.diffuse[0],
		// 							 spotLightComponent.diffuse[1],
		// 							 spotLightComponent.diffuse[2]); // darken diffuse light a bit
		// 	_Shader_Program->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
		// 							 spotLightComponent.specular[0],
		// 							 spotLightComponent.specular[1],
		// 							 spotLightComponent.specular[2]);
		// 	_Shader_Program->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].constant"),
		// 							  spotLightComponent.constant);
		// 	_Shader_Program->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].linear"),
		// 							  spotLightComponent.linear);
		// 	_Shader_Program->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].quadratic"),
		// 							  spotLightComponent.quadratic);
		// }

		_Shader_Program->SetVec3("lightPos", lightPosition.m_vector[0], lightPosition.m_vector[1], lightPosition.m_vector[2]);
		_Shader_Program->SetMat4("aLight_Space_Matrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glActiveTexture(GL_TEXTURE8);
//		glBindTexture(GL_TEXTURE_2D, texture_load_data_[0].entitiesOwnsThisTypeOfTexture_[0]);
	    glBindTexture(GL_TEXTURE_2D, 5);
		// Matrix<float, 4> model(1.0f);
		// unsigned int uiTransformt_Loc_Model = pGLGet_Uniform_Location(_Shader_Program->iID, "aModel_Matrix");
		// pGLUniform_Matrix4fv(uiTransformt_Loc_Model, NUMBER_OF_MATRICES, GL_FALSE, &model[0][0]);
		// pGLBind_Vertex_Array(planeVAO_);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		RenderScene(_Shader_Program);

		// debugQuadDepth_->Use();
		// debugQuadDepth_->SetFloat("nearPlane", nearPlane);
		// debugQuadDepth_->SetFloat("farPlane", farPlane);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, depthMapTexture);

		// RenderQuad();
	}

	void COpenglRenderer::RenderScene(Shader* _shader) {
		ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();

		for(int i = 0; i < texture_load_data_.size(); ++i)
			for (int j = 0; j < texture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = texture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).vkVertexId_;
				unsigned int diffuseTextureID = pComponent_Manager->GetComponent<ECS::SMaterialComponent>(uiEntity_refTexture).diffuseTextureID_;
				unsigned int specularTextureID = pComponent_Manager->GetComponent<ECS::SMaterialComponent>(uiEntity_refTexture).specularTextureID_;
				SetModelMatrix(_shader, pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				pGLActive_Texture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture_load_data_[diffuseTextureID].iTexture_);
				pGLActive_Texture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture_load_data_[specularTextureID].iTexture_);
				pGLBind_Vertex_Array(VAOcontainer_[uiVertexId]);
				ECS::SMaterialComponent& materialComponent = pComponent_Manager->GetComponent<ECS::SMaterialComponent>(uiEntity_refTexture);
				_Shader_Program->SetFloat("material.shininess", materialComponent.shininess);
				_Shader_Program->SetVec3("material.ambient",  materialComponent.ambient[0], materialComponent.ambient[1], materialComponent.ambient[2]);
				// _Shader_Program->SetVec3("material.diffuse",  materialComponent.diffuse[0], materialComponent.diffuse[1], materialComponent.diffuse[2]); // darken diffuse light a bit
//				_Shader_Program->SetVec3("material.specular", materialComponent.specular[0], materialComponent.specular[1], materialComponent.specular[2]);
				
				glDrawElements(GL_TRIANGLES, aIndices_[uiVertexId].size(), GL_UNSIGNED_INT, 0);
			}

		for(int i = 0; i < hudTexture_load_data_.size(); ++i)
			for (int j = 0; j < hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).vkVertexId_;
//				LoadTextureData(hudTexture_load_data_[pComponent_Manager->GetComponent<ECS::SMaterialComponent>(uiEntity_refTexture).diffuseTextureID_]);
				SetModelMatrix(_Shader_Program, pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				pGLActive_Texture(GL_TEXTURE5);
				pGLBind_Vertex_Array(VAOcontainer_[uiVertexId]);
				glDrawElements(GL_TRIANGLES, aIndices_[uiVertexId].size(), GL_UNSIGNED_INT, 0);
			}
	}

	void COpenglRenderer::RenderQuad()
	{
		if (quadVAO_ == 0)
			{
				float quadVertices[] = {
					// positions        // texture Coords
					-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
					1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				};
				// setup plane VAO
				pGLGen_Vertex_Arrays(1, &quadVAO_);
				pGLGen_Buffers(1, &quadVBO_);
				pGLBind_Vertex_Array(quadVAO_);
				pGLBind_Buffer(GL_ARRAY_BUFFER, quadVBO_);
				pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
				pGLEnable_Vertex_Attrib_Array(0);
				pGLVertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				pGLEnable_Vertex_Attrib_Array(1);
				pGLVertex_Attrib_Pointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			}
		pGLBind_Vertex_Array(quadVAO_);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		pGLBind_Vertex_Array(0);
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
		pGLVertex_Attrib_Pointer(LAYOUT_1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(LAYOUT_1);
		pGLVertex_Attrib_Pointer(2, TEXTURE_SIZE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(2);
        // pGLVertex_Attrib_Pointer(LAYOUT_0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)VERTEX_OFFSET);
        // pGLEnable_Vertex_Attrib_Array(LAYOUT_0);
		// pGLVertex_Attrib_Pointer(LAYOUT_1, TEXTURE_SIZE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(TEXTURE_OFFSET * sizeof(float)));
		// pGLEnable_Vertex_Attrib_Array(LAYOUT_1);
		// pGLVertex_Attrib_Pointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		// pGLEnable_Vertex_Attrib_Array(2);

		VBOcontainer_.push_back(iVbo_);
		VAOcontainer_.push_back(iVao_);
		EBOcontainer_.push_back(iEbo_);
	}
	
    void COpenglRenderer::loadWavefrontObj() {
        for (int m = 0; m < pathsArray_.size(); ++m) {
            CWaveFrontObjParser parser;
            CWaveFrontObjParser* wavefrontObjParser = &parser;

            wavefrontObjParser->ReadFile(pathsArray_[m]);
            wavefrontObjParser->ParseFile();
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
					aVertexes_[m].push_back(normal[0]);
					aVertexes_[m].push_back(normal[1]);
					aVertexes_[m].push_back(normal[2]);
					aVertexes_[m].push_back(texture[0]);
					aVertexes_[m].push_back(texture[1]);
                }
			SetVertices(aIndices_[m], aVertexes_[m]);
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
