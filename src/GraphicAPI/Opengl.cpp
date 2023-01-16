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
#include <vulkan/vulkan_core.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GLVM::Core
{
    COpenglRenderer::COpenglRenderer()
	{
		coreShaderProgram           = new Shader("../GLshaders/CoreShader.vert", "../GLshaders/CoreShader.frag");
		flatShadowMapShaderProgram  = new Shader("../GLshaders/FlatShadowMap.vert", "../GLshaders/FlatShadowMap.frag");
 		cubeShadowMapShaderProgram  = new Shader("../GLshaders/CubeShadowMap.vert", "../GLshaders/CubeShadowMap.frag",
			                                     "../GLshaders/CubeShadowMap.geom");
		debugQuadDepth_             = new Shader("../GLshaders/DebugQuadDepth.vert", "../GLshaders/DebugQuadDepth.frag");

        glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		debugQuadDepth_->Use();
		debugQuadDepth_->SetInt("depthMap", 30);
		
		coreShaderProgram->Use();
		coreShaderProgram->SetInt("directionalLightFlatShadowMap", 24);
		coreShaderProgram->SetInt("material.diffuse", 25);
		coreShaderProgram->SetInt("material.specular", 26);

//		InitilizeCubeShadowMap();
		unsigned int pointLightSourcesNumber = 16;

		for ( int i = 0; i < pointLightSourcesNumber; ++i ) {
			pointLightCubeShadowMapFBOcontainer.emplace_back();
			pointLightCubeShadowMapTextureContainer.emplace_back();
			InitializeShadowMapData(pointLightCubeShadowMapFBOcontainer[i], pointLightCubeShadowMapTextureContainer[i],
									GL_TEXTURE_CUBE_MAP, GL_CLAMP_TO_EDGE);
		}
		int pointLightArrayUniformIndices[pointLightSourcesNumber];
		for ( int i = 0; i < pointLightSourcesNumber; ++i )
			pointLightArrayUniformIndices[i] = i;

		coreShaderProgram->Use();
		coreShaderProgram->SetInt("pointLightCubeShadowMapArray", pointLightSourcesNumber, pointLightArrayUniformIndices);

 		unsigned int spotLightSourcesNumber = 8;

		for ( int i = 0; i < spotLightSourcesNumber; ++i ) {
			spotLightFlatShadowMapFBOContainer.emplace_back();
			spotLightFlatShadowMapTextureContainer.emplace_back();
			InitializeShadowMapData(spotLightFlatShadowMapFBOContainer[i], spotLightFlatShadowMapTextureContainer[i],
									GL_TEXTURE_2D, GL_CLAMP_TO_BORDER);
		}
		unsigned int spotLightCouner = 0;
		int spotLightArrayUniformIndices[spotLightSourcesNumber];
		for ( int i = 16; i < (pointLightSourcesNumber + spotLightSourcesNumber); ++i ) {
			spotLightArrayUniformIndices[spotLightCouner] = i;
			++spotLightCouner;
		}

		coreShaderProgram->Use();
		coreShaderProgram->SetInt("spotLightFlatShadowMapArray", spotLightSourcesNumber, spotLightArrayUniformIndices);

		InitializeShadowMapData(directionalLightFlatShadowMapFBO, directionalLightFlatShadowMapTexture,
								GL_TEXTURE_2D, GL_CLAMP_TO_BORDER);
		
//		InitializeFlatShadowMap();

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	COpenglRenderer::~COpenglRenderer()
	{
        delete coreShaderProgram;
        coreShaderProgram = nullptr;
		delete flatShadowMapShaderProgram;
		flatShadowMapShaderProgram = nullptr;
		delete cubeShadowMapShaderProgram;
		cubeShadowMapShaderProgram = nullptr;

		for (int i = 0; i < VBOcontainer_.size(); ++i)
			pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &VBOcontainer_[i]);
		
		for (int i = 0; i < VAOcontainer_.size(); ++i)
			pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &VAOcontainer_[i]);
		
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &quadVBO_);
		pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &quadVAO_);
	}
    
	void COpenglRenderer::draw() {
		ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefDirectionalLight = ECS::GetInnerIDsContainer<Core::SDirectionalLightComponent>(*pComponent_Manager);
		unsigned int uiDirectionalLightsEntity = (*pEntityContainerRefDirectionalLight)[0];
		Core::SDirectionalLightComponent& directionalLightComponent = pComponent_Manager->GetComponent<Core::SDirectionalLightComponent>(uiDirectionalLightsEntity);

		/// Player
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefView = ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
		unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		ECS::CViewComponent& playerViewComponent = pComponent_Manager->GetComponent<ECS::CViewComponent>(uiPlayerEntity);
		ECS::STransformComponent& playerTransformComponent = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiPlayerEntity);

		Core::TCVectorContainer<unsigned int>* pEntityContainerRefPointLight = ECS::GetInnerIDsContainer<Core::SPointLightComponent>(*pComponent_Manager);
		
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		vec3 positionVectorDirectionalLight = directionalLightComponent.position;
		vec3 directionVectorDirectionalLight = directionalLightComponent.direction;
//		vec3 positionVectorDirectionalLight = playerTransformComponent.tPosition;
		vec3 upVectorDirectionalLight        = { 0.0f, 1.0f, 0.0f };
		mat4 projectionMatrixDirectionalLight = ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlaneFlatShadowMap, farPlaneFlatShadowMap);
		mat4 viewMatrixDirectionalLight = LookAtMain(positionVectorDirectionalLight,
													 directionVectorDirectionalLight,
													 { 0.0f, 1.0f, 0.0f });
		directionalLightSpaceMatrix = viewMatrixDirectionalLight * projectionMatrixDirectionalLight;
		// Render scene from light's point of view
		flatShadowMapShaderProgram->Use();
		EvaluateFlatShadowMap("lightSpaceMatrix", directionalLightSpaceMatrix, directionalLightFlatShadowMapFBO, GL_TEXTURE0);

		Core::TCVectorContainer<unsigned int>* pEntityContainerRefSpotLight = ECS::GetInnerIDsContainer<ECS::SSpotLightComponent>(*pComponent_Manager);
		std::string leftString = "spotLightFlatShadowMapComponentIndices[";
		unsigned int appropriateLightComponentIndex = 0;
		spotLightComponentContainerSize = pEntityContainerRefSpotLight->GetSize();
		for(int x = 0; x < spotLightComponentContainerSize; ++x) {
			unsigned int uiSpotLightEntity = (*pEntityContainerRefSpotLight)[x];
			ECS::SSpotLightComponent& spotLightComponent = pComponent_Manager->GetComponent<ECS::SSpotLightComponent>(uiSpotLightEntity);
			vec3 positionVectorSpotLight  = spotLightComponent.position;
			vec3 directionVectorSpotLight = spotLightComponent.direction;
			vec3 upVectorSpotLight        = { 0.0f, 1.0f, 0.0f };
//			mat4 projectionMatrixSpotLight = Perspective(Radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlaneFlatShadowMap, farPlaneFlatShadowMap);
			mat4 projectionMatrixSpotLight = ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlaneFlatShadowMap, farPlaneFlatShadowMap);
			mat4 viewMatrixSpotLight = LookAtMain(positionVectorSpotLight,
														 directionVectorSpotLight,
														 upVectorSpotLight);

			mat4 spotLightSpaceMatrix = viewMatrixSpotLight * projectionMatrixSpotLight;
			flatShadowMapShaderProgram->Use();
			EvaluateFlatShadowMap("lightSpaceMatrix", spotLightSpaceMatrix, spotLightFlatShadowMapFBOContainer[appropriateLightComponentIndex], GL_TEXTURE10);
			spotLightSpaceMatrixContainer[appropriateLightComponentIndex] = spotLightSpaceMatrix;

			coreShaderProgram->Use();
			coreShaderProgram->SetInt(ConcatIntBetweenTwoStrings(leftString, appropriateLightComponentIndex, "]"), x);
				++appropriateLightComponentIndex;
		}

		coreShaderProgram->Use();
		coreShaderProgram->SetMat4("spotLightSpaceMatrixContainer", appropriateLightComponentIndex, spotLightSpaceMatrixContainer[0]);
		coreShaderProgram->SetInt("spotLightSpaceMatrixContainerSize", appropriateLightComponentIndex);
		spotLightComponentContainerSize = appropriateLightComponentIndex;
		
// 		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
// 		debugQuadDepth_->Use();
// 		debugQuadDepth_->SetFloat("nearPlane", nearPlaneFlatShadowMap);
// 		debugQuadDepth_->SetFloat("fatPlane", farPlaneFlatShadowMap);
// 		glActiveTexture(GL_TEXTURE30);
// 		glBindTexture(GL_TEXTURE_2D, spotLightFlatShadowMapFBOContainer[0]);
// //		glBindTexture(GL_TEXTURE_2D, directionalLightFlatShadowMapTexture);
// 		RenderQuad();
		
		EvaluateCubeShadowMap();		
		coreShaderProgram->Use();
		
		ComputeDirectionalLight();
		ComputePointLight();
		ComputeSpotLight();
 
		
	    EvaluateCoreShader();
		RenderScene(coreShaderProgram);

		Window.SwapBuffers();
	}

	void COpenglRenderer::InitializeShadowMapData(unsigned int& fbo_, unsigned int& texture_, GLenum textureTarget_,
												  GLint clampType_) {
				pGLGen_Framebuffers(1, &fbo_);
				glGenTextures(1, &texture_);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(textureTarget_, texture_);
				AllocateTexture(textureTarget_, clampType_);
				
				// int currentActiveTexture;
				// glGetIntegerv(GL_ACTIVE_TEXTURE, &currentActiveTexture);
				// std::cout << "Cube texture unit: " << currentActiveTexture << std::endl;
				
				// Attach depth texture as FBO's depth buffer
				pGLBind_Framebuffer(GL_FRAMEBUFFER, fbo_);
				pGLFramebuffer_Texture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);
	}

	void COpenglRenderer::AllocateTexture ( GLenum textureTarget_, GLint clampType_ ) {
		switch ( textureTarget_ ) {
		case GL_TEXTURE_2D:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampType_); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampType_);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			break;
		case GL_TEXTURE_CUBE_MAP:
			for (unsigned int j = 0; j < 6; ++j)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, clampType_); 
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, clampType_);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, clampType_);
		break;
		default:
			break;
		}
	}
	
	void COpenglRenderer::EvaluateFlatShadowMap(std::string uniformLayout, mat4 lightSpaceMatrix,
		                                        unsigned int& fbo_, GLenum textureUnit_) {
		flatShadowMapShaderProgram->SetMat4(uniformLayout, lightSpaceMatrix);
		// Render to depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		pGLBind_Framebuffer(GL_FRAMEBUFFER, fbo_);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(textureUnit_);
//		glCullFace(GL_FRONT);
		RenderScene(flatShadowMapShaderProgram);
//		glCullFace(GL_BACK);
		pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);
//		}
	}

	void COpenglRenderer::EvaluateCubeShadowMap() {
		// ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
		// Core::TCVectorContainer<unsigned int>* pEntityContainerRefView = ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
		// unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		// ECS::CViewComponent& playerViewComponent = pComponent_Manager->GetComponent<ECS::CViewComponent>(uiPlayerEntity);
		// ECS::STransformComponent& playerTransformComponent = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiPlayerEntity);


		
        ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefView = ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
		unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		ECS::CViewComponent& playerViewComponent = pComponent_Manager->GetComponent<ECS::CViewComponent>(uiPlayerEntity);
		ECS::STransformComponent& playerTransformComponent = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiPlayerEntity);

		Core::TCVectorContainer<unsigned int>* pEntityContainerRefPointLight = ECS::GetInnerIDsContainer<Core::SPointLightComponent>(*pComponent_Manager);
		unsigned int pointLightComponentContainerSize = pEntityContainerRefPointLight->GetSize();
		std::string leftString = "pointLightCubeShadowMapComponentIndices[";

		sampledPointLightEntityIDcontainer.clear();
		

		unsigned int appropriateLightComponentIndex = 0;
		for ( int i = 0; i < pointLightComponentContainerSize; ++i) {
			unsigned int entityID = (*pEntityContainerRefPointLight)[i];
			Core::SPointLightComponent& pointLightComponent = pComponent_Manager->GetComponent<Core::SPointLightComponent>(entityID);
			float distance = VectorLength(playerTransformComponent.tPosition, pointLightComponent.position);

			if ( distance < 4.5f ) {
				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				sampledPointLightEntityIDcontainer.push_back(i);
				vec3 positionVectorPointLight = pointLightComponent.position;
//		positionVectorPointLight = playerTransformComponent.tPosition;
//		positionVectorPointLight = vec3(timeAccumulator * 5, 3.0f, timeAccumulator * 5);
				mat4 projectionMatrixCubeShadowMap = Perspective(Radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlaneCubeShadowMap, farPlaneCubeShadowMap);
				vector<mat4> cubeShadowMapTransforms;
				cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)) * projectionMatrixCubeShadowMap);
				cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( -1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)) * projectionMatrixCubeShadowMap);
				cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 0.0f,  1.0f,  0.0f), vec3(0.0f, 0.0f,  1.0f)) * projectionMatrixCubeShadowMap);
				cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 0.0f,  -1.0f,  0.0f), vec3(0.0f, 0.0f,  -1.0f)) * projectionMatrixCubeShadowMap);
				cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f)) * projectionMatrixCubeShadowMap);
				cubeShadowMapTransforms.Push(LookAtMain(positionVectorPointLight, positionVectorPointLight + vec3( 0.0f,  0.0f,  -1.0f), vec3(0.0f, -1.0f,  0.0f)) * projectionMatrixCubeShadowMap);

				glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
				pGLBind_Framebuffer(GL_FRAMEBUFFER, pointLightCubeShadowMapFBOcontainer[appropriateLightComponentIndex]);
				glClear(GL_DEPTH_BUFFER_BIT);
				cubeShadowMapShaderProgram->Use();
				for (unsigned int j = 0; j < 6; ++j)
					cubeShadowMapShaderProgram->SetMat4("shadowMatrices[" + std::to_string(j) + "]", cubeShadowMapTransforms[j]);
				cubeShadowMapShaderProgram->SetFloat("farPlane", farPlaneCubeShadowMap);
				cubeShadowMapShaderProgram->SetVec3("lightPosition", positionVectorPointLight);
				RenderScene(cubeShadowMapShaderProgram);
				pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);

				coreShaderProgram->Use();
				coreShaderProgram->SetInt(ConcatIntBetweenTwoStrings(leftString, appropriateLightComponentIndex, "]"), i);
				++appropriateLightComponentIndex;
			}
		}
		coreShaderProgram->Use();
		coreShaderProgram->SetInt("pointLightCubeShadowMapArraySize", appropriateLightComponentIndex);
	}

	void COpenglRenderer::EvaluateCoreShader() {
        ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefView = ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
		unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		ECS::CViewComponent& playerViewComponent = pComponent_Manager->GetComponent<ECS::CViewComponent>(uiPlayerEntity);
		ECS::STransformComponent& playerTransformComponent = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiPlayerEntity);
		
//		viewPosition = playerViewComponent.Position;
		vec3 viewPosition = playerTransformComponent.tPosition;
		
		// Render scene as normal
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		coreShaderProgram->Use();
		ComputeProjectionMatrix(coreShaderProgram);
		ComputeViewMatrix(coreShaderProgram, playerTransformComponent, playerViewComponent);
		coreShaderProgram->SetInt("shadows", shadows);
		coreShaderProgram->SetFloat("farPlane", farPlaneCubeShadowMap);
		coreShaderProgram->SetVec3("viewPosition", viewPosition);
		coreShaderProgram->SetMat4("directionalLightSpaceMatrix", directionalLightSpaceMatrix);
		coreShaderProgram->SetInt("spotLightSpaceMatrixContainerSize", spotLightComponentContainerSize);
		coreShaderProgram->SetMat4("spotLightSpaceMatrixContainer", spotLightComponentContainerSize,
								   spotLightSpaceMatrixContainer[0]);

		glActiveTexture(GL_TEXTURE24);
		glBindTexture(GL_TEXTURE_2D, directionalLightFlatShadowMapTexture);
		for ( int i = 0; i < sampledPointLightEntityIDcontainer.size(); ++i ) {
			glActiveTexture( GL_TEXTURE0 + i );
			glBindTexture( GL_TEXTURE_CUBE_MAP, pointLightCubeShadowMapTextureContainer[i] );
		}

		for ( int i = 0; i < sampledSpotLightEntityIDcontainer.size(); ++i ) {
			glActiveTexture( GL_TEXTURE16 + i );
			glBindTexture( GL_TEXTURE_2D, spotLightFlatShadowMapTextureContainer[i] );
		}
	}

	void COpenglRenderer::EvaluateFlatDebugShader() {
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		debugQuadDepth_->Use();
		debugQuadDepth_->SetFloat("nearPlane", nearPlaneFlatShadowMap);
		debugQuadDepth_->SetFloat("farPlane", farPlaneFlatShadowMap);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, directionalLightFlatShadowMapTexture);
	}
	
	void COpenglRenderer::ComputeDirectionalLight() {
		ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
				Core::TCVectorContainer<unsigned int>* pEntityContainerRefDirectionalLight = ECS::GetInnerIDsContainer<Core::SDirectionalLightComponent>(*pComponent_Manager);
		unsigned int directionalLightComponentContainerSize = pEntityContainerRefDirectionalLight->GetSize();

		coreShaderProgram->SetInt("directionalLightsArraySize", directionalLightComponentContainerSize);

		for(int x = 0; x < directionalLightComponentContainerSize; ++x) {
			unsigned int uiDirectionalLightEntity = (*pEntityContainerRefDirectionalLight)[x];
			Core::SDirectionalLightComponent& directionalLightComponent = pComponent_Manager->GetComponent<Core::SDirectionalLightComponent>(uiDirectionalLightEntity);
			std::string leftString = "directionalLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
									   directionalLightComponent.position);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].direction"),
										directionalLightComponent.direction);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
									   directionalLightComponent.ambient);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
									   directionalLightComponent.diffuse); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
									   directionalLightComponent.specular);

		}
	}

	void COpenglRenderer::ComputePointLight() {
		ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefPointLight = ECS::GetInnerIDsContainer<Core::SPointLightComponent>(*pComponent_Manager);
		unsigned int pointLightComponentContainerSize = pEntityContainerRefPointLight->GetSize();
		coreShaderProgram->SetInt("pointLightsArraySize", pointLightComponentContainerSize);
		for(int x = 0; x < pointLightComponentContainerSize; ++x) {
			unsigned int uiPointLightEntity = (*pEntityContainerRefPointLight)[x];
			Core::SPointLightComponent& pointLightComponent = pComponent_Manager->GetComponent<Core::SPointLightComponent>(uiPointLightEntity);
			
			// Core::TCVectorContainer<unsigned int>* pEntityContainerRefView = ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
			// unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
			// ECS::STransformComponent& playerTransformComponent = pComponent_Manager->GetComponent<ECS::STransformComponent>(uiPlayerEntity);

			std::string leftString = "pointLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
									   pointLightComponent.position);
			// coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
			// 						   playerTransformComponent.tPosition);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
									   pointLightComponent.ambient);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
									   pointLightComponent.diffuse); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
									   pointLightComponent.specular);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].constant"),
										pointLightComponent.constant);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].linear"),
										pointLightComponent.linear);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].quadratic"),
										pointLightComponent.quadratic);
		}
	}

	void COpenglRenderer::ComputeSpotLight() {
		ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
		Core::TCVectorContainer<unsigned int>* pEntityContainerRefSpotLight = ECS::GetInnerIDsContainer<ECS::SSpotLightComponent>(*pComponent_Manager);
		unsigned int spotLightComponentContainerSize = pEntityContainerRefSpotLight->GetSize();
		coreShaderProgram->SetInt("spotLightsArraySize", spotLightComponentContainerSize);
		for(int x = 0; x < spotLightComponentContainerSize; ++x) {
			unsigned int uiSpotLightEntity = (*pEntityContainerRefSpotLight)[x];
			ECS::SSpotLightComponent& spotLightComponent = pComponent_Manager->GetComponent<ECS::SSpotLightComponent>(uiSpotLightEntity);
			std::string leftString = "spotLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
									   spotLightComponent.position);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].direction"),
									   spotLightComponent.direction);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].cutOff"),
										std::cos(Radians(spotLightComponent.cutOff)));
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].outerCutOff"),
										std::cos(Radians(spotLightComponent.outerCutOff)));
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
									   spotLightComponent.ambient);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
									   spotLightComponent.diffuse); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
									   spotLightComponent.specular);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].constant"),
										spotLightComponent.constant);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].linear"),
										spotLightComponent.linear);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].quadratic"),
										spotLightComponent.quadratic);
		}
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
				shaderProgram_->SetMat4("modelMatrix", modelMatrix);
				pGLActive_Texture(GL_TEXTURE25);
				glBindTexture(GL_TEXTURE_2D, texture_load_data_[diffuseTextureID].iTexture_);
				pGLActive_Texture(GL_TEXTURE26);
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
				modelMatrix = SetModelMatrix(pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTexture));
				shaderProgram_->SetMat4("modelMatrix", modelMatrix);
				pGLActive_Texture(GL_TEXTURE30);
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
        const float kSensitivity = 0.1f;

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


 		// _view_Component.Position[0] = _Player.tPosition[0];
		// _view_Component.Position[1] = _Player.tPosition[1];
		// _view_Component.Position[2] = _Player.tPosition[2];

		shaderProgram->SetMat4("viewMatrix", tView_Matrix);
    }

	void COpenglRenderer::ComputeProjectionMatrix(Shader* shaderProgram) {
		mat4 tProjection_Matrix = Perspective(Radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
		shaderProgram->SetMat4("projectionMatrix", tProjection_Matrix);
	}
}
