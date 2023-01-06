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
#include <X11/X.h>
#include <cmath>
#include "Globals.hpp"
#include "WavefrontObjParser.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <glm/trigonometric.hpp>
#include <ratio>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GLVM::Core
{
    COpenglRenderer::COpenglRenderer()
	{
		coreShaderProgram           = new Shader("../GLshaders/CoreShader.vert", "../GLshaders/CoreShader.frag");
		// coreShaderProgram           = new Shader("/home/cyberdemon/cyberDemonCode/Opengl/shadowMappingPointLights/src/point_shadows.vs", "/home/cyberdemon/cyberDemonCode/Opengl/shadowMappingPointLights/src/point_shadows.fs");
		flatShadowMapShaderProgram = new Shader("../GLshaders/FlatShadowMap.vert", "../GLshaders/FlatShadowMap.frag");
 		cubeShadowMapShaderProgram  = new Shader("../GLshaders/CubeShadowMap.vert", "../GLshaders/CubeShadowMap.frag",
			                                     "../GLshaders/CubeShadowMap.geom");
		// cubeShadowMapShaderProgram  = new Shader("/home/cyberdemon/cyberDemonCode/Opengl/shadowMappingPointLights/src/point_shadows_depth.vs", "/home/cyberdemon/cyberDemonCode/Opengl/shadowMappingPointLights/src/point_shadows_depth.fs", "/home/cyberdemon/cyberDemonCode/Opengl/shadowMappingPointLights/src/point_shadows_depth.gs");
		debugQuadDepth_             = new Shader("../GLshaders/DebugQuadDepth.vert", "../GLshaders/DebugQuadDepth.frag");
		
		woodTexture = loadTexture("/home/cyberdemon/cyberDemonCode/GLVMEngine/textures/container2.png");

        glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		pGLGen_Framebuffers(1, &flatShadowMapFBO);
		glGenTextures(1, &flatShadowMapTexture);
		glBindTexture(GL_TEXTURE_2D, flatShadowMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		pGLBind_Framebuffer(GL_FRAMEBUFFER, flatShadowMapFBO);
		pGLFramebuffer_Texture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, flatShadowMapTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);		
		
		pGLGen_Framebuffers(1, &cubeShadowMapFBO);
		glGenTextures(1, &cubeShadowMapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeShadowMapTexture);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // Attach depth texture as FBO's depth buffer
		pGLBind_Framebuffer(GL_FRAMEBUFFER, cubeShadowMapFBO);
		pGLFramebuffer_Texture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeShadowMapTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);

		coreShaderProgram->Use();
		coreShaderProgram->SetInt("flatShadowMap", 0);
		coreShaderProgram->SetInt("cubeShadowMap", 1);
		coreShaderProgram->SetInt("diffuseTexture", 2);
		coreShaderProgram->SetInt("material.diffuse", 3);
		coreShaderProgram->SetInt("material.specular", 4);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	COpenglRenderer::~COpenglRenderer()
	{
        delete coreShaderProgram;
        coreShaderProgram = nullptr;

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
		ECS::STransformComponent& playerTransformComponent = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiPlayerEntity);

		if ( timeAccumulator > 1.0f ) {
			timeFlag = true;
		} else if ( timeAccumulator < 0.0f ) {
			timeFlag = false;
		}

		if (timeFlag)
			timeAccumulator -= 0.001;
		else
			timeAccumulator += 0.001;	
		
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefDirectionalLight = ECS::GetInnerIDsContainer<Core::SDirectionalLightComponent>(*pComponent_Manager);
		unsigned int directionalLightComponentContainerSize = pEntityContainerRefDirectionalLight->GetSize();

		// glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 		float nearPlaneFlatShadowMap = 1.0f, farPlaneFlatShadowMap = 25.0f;
// 		vec3 positionVectorDirectionalLight = playerTransformComponent.tPosition;
// 		vec3 viewPosition(playerViewComponent.Position.m_vector[0], playerViewComponent.Position.m_vector[1], playerViewComponent.Position.m_vector[2]);
// //		vec3 positionVectorDirectionalLight  = { 5.0f, 5.0f, 1.0f };
// 		vec3 directionVectorDirectionalLight = { 0.0f, 0.0f, 0.0f };
// 		vec3 upVectorDirectionalLight        = { 0.0f, 1.0f, 0.0f };
// 		// for(int x = 0; x < directionalLightComponentContainerSize; ++x) {
// 		// 	unsigned int uiDirectionalLightEntity = (*pEntityContainerRefDirectionalLight)[x];
// 		// 	Core::SDirectionalLightComponent& directionalLightComponent = pComponent_Manager->GetComponent<Core::SDirectionalLightComponent>(uiDirectionalLightEntity);
// 			mat4 projectionMatrixDirectionalLight = ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlaneFlatShadowMap, farPlaneFlatShadowMap);
// 			mat4 viewMatrixDirectionalLight = LookAtMain(positionVectorDirectionalLight,
// 																	  directionVectorDirectionalLight,
// 																	  upVectorDirectionalLight);
// 			mat4 lightSpaceMatrix = viewMatrixDirectionalLight * projectionMatrixDirectionalLight;
// 			// Render scene from light's point of view
// 			flatShadowMapShaderProgram->Use();
// 			flatShadowMapShaderProgram->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
// 			// Render to depth map
// 			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
// 			pGLBind_Framebuffer(GL_FRAMEBUFFER, flatShadowMapFBO);
// 			glClear(GL_DEPTH_BUFFER_BIT);
// 			RenderScene(flatShadowMapShaderProgram);
// 			pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);
//		}
		
		vec3 viewPosition(playerViewComponent.Position.m_vector[0], playerViewComponent.Position.m_vector[1], playerViewComponent.Position.m_vector[2]);
		coreShaderProgram->SetVec3("viewPosition", viewPosition);
		coreShaderProgram->SetInt("directionalLightsArraySize", directionalLightComponentContainerSize);
		for(int x = 0; x < directionalLightComponentContainerSize; ++x) {
			unsigned int uiDirectionalLightEntity = (*pEntityContainerRefDirectionalLight)[x];
			Core::SDirectionalLightComponent& directionalLightComponent = pComponent_Manager->GetComponent<Core::SDirectionalLightComponent>(uiDirectionalLightEntity);
			std::string leftString = "directionalLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].direction"), directionalLightComponent.direction[0], directionalLightComponent.direction[1], directionalLightComponent.direction[2]);

			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),  directionalLightComponent.ambient[0], directionalLightComponent.ambient[1], directionalLightComponent.ambient[2]);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),  directionalLightComponent.diffuse[0], directionalLightComponent.diffuse[1], directionalLightComponent.diffuse[2]); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"), directionalLightComponent.specular[0], directionalLightComponent.specular[1], directionalLightComponent.specular[2]);
		}

		Core::TCVectorContainer<unsigned int>* pEntityContainerRefPointLight = ECS::GetInnerIDsContainer<Core::SPointLightComponent>(*pComponent_Manager);
		unsigned int pointLightComponentContainerSize = pEntityContainerRefPointLight->GetSize();
		coreShaderProgram->SetInt("pointLightsArraySize", pointLightComponentContainerSize);
		for(int x = 0; x < pointLightComponentContainerSize; ++x) {
			unsigned int uiPointLightEntity = (*pEntityContainerRefPointLight)[x];
			Core::SPointLightComponent& pointLightComponent = pComponent_Manager->GetComponent<Core::SPointLightComponent>(uiPointLightEntity);
			std::string leftString = "pointLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
									 pointLightComponent.position[0],
									 pointLightComponent.position[1],
									 pointLightComponent.position[2]);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
									 pointLightComponent.ambient[0],
									 pointLightComponent.ambient[1],
									 pointLightComponent.ambient[2]);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
									 pointLightComponent.diffuse[0],
									 pointLightComponent.diffuse[1],
									 pointLightComponent.diffuse[2]); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
									 pointLightComponent.specular[0],
									 pointLightComponent.specular[1],
									 pointLightComponent.specular[2]);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].constant"),
									  pointLightComponent.constant);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].linear"),
									  pointLightComponent.linear);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].quadratic"),
									  pointLightComponent.quadratic);
		}

		Core::TCVectorContainer<unsigned int>* pEntityContainerRefSpotLight = ECS::GetInnerIDsContainer<ECS::SSpotLightComponent>(*pComponent_Manager);
		unsigned int spotLightComponentContainerSize = pEntityContainerRefSpotLight->GetSize();
		coreShaderProgram->SetInt("spotLightsArraySize", spotLightComponentContainerSize);
		for(int x = 0; x < spotLightComponentContainerSize; ++x) {
			unsigned int uiSpotLightEntity = (*pEntityContainerRefSpotLight)[x];
			ECS::SSpotLightComponent& spotLightComponent = pComponent_Manager->GetComponent<ECS::SSpotLightComponent>(uiSpotLightEntity);
			std::string leftString = "spotLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
									 spotLightComponent.position[0],
									 spotLightComponent.position[1],
									 spotLightComponent.position[2]);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].direction"),
									 spotLightComponent.direction[0],
									 spotLightComponent.direction[1],
									 spotLightComponent.direction[2]);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].cutOff"),
									  std::cos(Radians(spotLightComponent.cutOff)));
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].outerCutOff"),
									  std::cos(Radians(spotLightComponent.outerCutOff)));
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
									 spotLightComponent.ambient[0],
									 spotLightComponent.ambient[1],
									 spotLightComponent.ambient[2]);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
									 spotLightComponent.diffuse[0],
									 spotLightComponent.diffuse[1],
									 spotLightComponent.diffuse[2]); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
									 spotLightComponent.specular[0],
									 spotLightComponent.specular[1],
									 spotLightComponent.specular[2]);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].constant"),
									  spotLightComponent.constant);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].linear"),
									  spotLightComponent.linear);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].quadratic"),
									  spotLightComponent.quadratic);
		}
		
		// glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// float nearPlaneCubeShadowMap = 1.0f;
		// float farPlaneCubeShadowMap  = 25.0f;
		// positionVectorPointLight = vec3(timeAccumulator * 5, 3.0f, timeAccumulator * 5);
		// mat4 projectionMatrixCubeShadowMap = Perspective(Radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlaneCubeShadowMap, farPlaneCubeShadowMap);
        // vector<mat4> cubeShadowMapTransforms;
        // cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)) * projectionMatrixCubeShadowMap);
		// cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( -1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)) * projectionMatrixCubeShadowMap);
		// cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 0.0f,  1.0f,  0.0f), vec3(0.0f, 0.0f,  1.0f)) * projectionMatrixCubeShadowMap);
		// cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 0.0f,  -1.0f,  0.0f), vec3(0.0f, 0.0f,  -1.0f)) * projectionMatrixCubeShadowMap);
		// cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f)) * projectionMatrixCubeShadowMap);
		// cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 0.0f,  0.0f,  -1.0f), vec3(0.0f, -1.0f,  0.0f)) * projectionMatrixCubeShadowMap);
		
		// glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		// pGLBind_Framebuffer(GL_FRAMEBUFFER, cubeShadowMapFBO);
		// glClear(GL_DEPTH_BUFFER_BIT);
		// cubeShadowMapShaderProgram->Use();
		// for (unsigned int i = 0; i < 6; ++i)
        //         cubeShadowMapShaderProgram->SetMat4("shadowMatrices[" + std::to_string(i) + "]", cubeShadowMapTransforms[i]);
		// cubeShadowMapShaderProgram->SetFloat("farPlane", farPlaneCubeShadowMap);
		// cubeShadowMapShaderProgram->SetVec3("lightPosition", positionVectorPointLight);
		// RenderScene(cubeShadowMapShaderProgram);
		// pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);

		// // Render scene as normal
		// glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// coreShaderProgram->Use();
		// ComputeProjectionMatrix(coreShaderProgram);
		// ComputeViewMatrix(coreShaderProgram, playerTransformComponent, playerViewComponent);
		// coreShaderProgram->SetVec3("lightPos", positionVectorPointLight);
		// coreShaderProgram->SetVec3("viewPosition", viewPosition);
		// coreShaderProgram->SetInt("shadows", shadows);
		// coreShaderProgram->SetFloat("farPlane", farPlaneCubeShadowMap);
		// coreShaderProgram->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, flatShadowMapTexture);
		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, cubeShadowMapTexture);
		// glActiveTexture(GL_TEXTURE2);
		// glBindTexture(GL_TEXTURE_2D, woodTexture);
		RenderScene(coreShaderProgram);

		Window.SwapBuffers();
		// debugQuadDepth_->Use();
		// debugQuadDepth_->SetFloat("nearPlane", nearPlane);
		// debugQuadDepth_->SetFloat("farPlane", farPlane);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, depthMapTexture);

		// RenderQuad();
	}

	void COpenglRenderer::RenderScene(Shader* shaderProgram_) {
		ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
		mat4 modelMatrix(1.0f);
			
		for(int i = 0; i < texture_load_data_.size(); ++i)
			for (int j = 0; j < texture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = texture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).vkVertexId_;
				unsigned int diffuseTextureID = pComponent_Manager->GetComponent<ECS::SMaterialComponent>(uiEntity_refTexture).diffuseTextureID_;
				unsigned int specularTextureID = pComponent_Manager->GetComponent<ECS::SMaterialComponent>(uiEntity_refTexture).specularTextureID_;
				modelMatrix = SetModelMatrix(pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				// if (j != 1)
				// 	modelMatrix = Translate(modelMatrix, vec3(timeAccumulator * 5, 0.0f, 0.0f));
				shaderProgram_->SetMat4("modelMatrix", modelMatrix);
				pGLActive_Texture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, texture_load_data_[diffuseTextureID].iTexture_);
				pGLActive_Texture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, texture_load_data_[specularTextureID].iTexture_);
				pGLBind_Vertex_Array(VAOcontainer_[uiVertexId]);
				ECS::SMaterialComponent& materialComponent = pComponent_Manager->GetComponent<ECS::SMaterialComponent>(uiEntity_refTexture);
				shaderProgram_->SetFloat("material.shininess", materialComponent.shininess);
				shaderProgram_->SetVec3("material.ambient",  materialComponent.ambient[0], materialComponent.ambient[1], materialComponent.ambient[2]);
				// coreShaderProgram->SetVec3("material.diffuse",  materialComponent.diffuse[0], materialComponent.diffuse[1], materialComponent.diffuse[2]); // darken diffuse light a bit
//				coreShaderProgram->SetVec3("material.specular", materialComponent.specular[0], materialComponent.specular[1], materialComponent.specular[2]);
				glDrawElements(GL_TRIANGLES, aIndices_[uiVertexId].size(), GL_UNSIGNED_INT, 0);
			}

		for(int i = 0; i < hudTexture_load_data_.size(); ++i)
			for (int j = 0; j < hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<ECS::SVertexComponent>(uiEntity_refTexture).vkVertexId_;
//				LoadTextureData(hudTexture_load_data_[pComponent_Manager->GetComponent<ECS::SMaterialComponent>(uiEntity_refTexture).diffuseTextureID_]);
				modelMatrix = SetModelMatrix(pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				shaderProgram_->SetMat4("modelMatrix", modelMatrix);
				pGLActive_Texture(GL_TEXTURE3);
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
    
	mat4 COpenglRenderer::SetModelMatrix(ECS::STransformComponent& transformComponent_)
	{
        mat4 rotationMatrix(1.0f);
        mat4 modelMatrix(1.0f);
        mat4 scalingMatrix(1.0f);
        mat4 translationMatrix(1.0f);

        scalingMatrix[0][0] = transformComponent_.fScale;
        scalingMatrix[1][1] = transformComponent_.fScale;
        scalingMatrix[2][2] = transformComponent_.fScale;
		scalingMatrix[3][3] = 1.0f;
        
        translationMatrix[3][0] = transformComponent_.tPosition[0];
		translationMatrix[3][1] = transformComponent_.tPosition[1];
		translationMatrix[3][2] = transformComponent_.tPosition[2];
        translationMatrix[3][3] = 1.0f;
		
        modelMatrix = scalingMatrix * translationMatrix;

		return modelMatrix;
	}

    void COpenglRenderer::SetViewMatrix(mat4 _viewMatrix) {
        unsigned int uniformLocationViewWorld = pGLGet_Uniform_Location(coreShaderProgram->iID, "viewMatrix");
        pGLUniform_Matrix4fv(uniformLocationViewWorld, NUMBER_OF_MATRICES, GL_FALSE, &_viewMatrix[0][0]);
    }

    void COpenglRenderer::SetProjectionMatrix(mat4 _projectionMatrix) {
        unsigned int uniformLocationProjectionWorld = pGLGet_Uniform_Location(coreShaderProgram->iID, "projectionMatrix");
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

	void COpenglRenderer::ComputeViewMatrix(Shader* shaderProgram, ECS::STransformComponent& _Player, ECS::CViewComponent& _view_Component)
    {
        Matrix<float, 4> tView_Matrix(1.0f);
        const float kSensitivity = 0.05f;

        fYaw = g_eEvent.mouse_Pointer_Position_.iOffset_X;
        fPitch = g_eEvent.mouse_Pointer_Position_.iOffset_Y;
        fYaw *= kSensitivity;
        fPitch *= kSensitivity;

        g_eEvent.mouse_Pointer_Position_.fPitch_ = fPitch;
        g_eEvent.mouse_Pointer_Position_.fYaw_ = fYaw;
        
        if(fPitch > 89.0f)
            fPitch = 89.0f;
        if(fPitch < -89.0f)
            fPitch = -89.0f;
		
		vec3 front;
        front[0] = std::cos(Radians(fYaw)) * std::cos(Radians(fPitch));
        front[1] = std::sin(Radians(fPitch));
        front[2] = std::sin(Radians(fYaw)) * std::cos(Radians(fPitch));
        _view_Component.Front_Camera = Normalize(front);

        tView_Matrix = LookAtMain(_Player.tPosition,
								  _Player.tPosition + _view_Component.Front_Camera,
								  _view_Component.Up_Camera);

 		_view_Component.Position[0] = _Player.tPosition[0];
		_view_Component.Position[1] = _Player.tPosition[1];
		_view_Component.Position[2] = _Player.tPosition[2];

		shaderProgram->SetMat4("viewMatrix", tView_Matrix);
    }

	void COpenglRenderer::ComputeProjectionMatrix(Shader* shaderProgram) {
		mat4 tProjection_Matrix = Perspective(Radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
		shaderProgram->SetMat4("projectionMatrix", tProjection_Matrix);
	}

	unsigned int COpenglRenderer::loadTexture(char const * path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
			{
				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				pGLGenerate_Mipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
		else
			{
				std::cout << "Texture failed to load at path: " << path << std::endl;
				stbi_image_free(data);
			}

		return textureID;
	}

}
