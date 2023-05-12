#include "GraphicAPI/Opengl.hpp"
#include "ComponentManager.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Constants.hpp"
#include "Engine.hpp"
#include "Event.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "GLPointer.h"
#include "GraphicAPI/Vulkan.hpp"
#include "JsonParser.hpp"
#include "MeshManager.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "TextureManager.hpp"
#include "ToString.hpp"
#include "Vector.hpp"
#include "Components/VertexComponent.hpp"
#include "VertexData.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <X11/Xlib.h>
#include <cmath>
#include "Globals.hpp"
#include "WavefrontObjParser.hpp"

#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <math.h>
#include <ratio>
#include <sstream>
#include <thread>

namespace GLVM::core
{
    COpenglRenderer::COpenglRenderer()
	{
		coreShaderProgram           = new Shader("../GLshaders/CoreShader.vert", "../GLshaders/CoreShader.frag");
		flatShadowMapShaderProgram  = new Shader("../GLshaders/FlatShadowMap.vert", "../GLshaders/FlatShadowMap.frag");
 		cubeShadowMapShaderProgram  = new Shader("../GLshaders/CubeShadowMap.vert", "../GLshaders/CubeShadowMap.frag",
			                                     "../GLshaders/CubeShadowMap.geom");
		debugQuadDepth_             = new Shader("../GLshaders/DebugQuadDepth.vert", "../GLshaders/DebugQuadDepth.frag");
		debugLines                  = new Shader("../GLshaders/debugLines.vert", "../GLshaders/debugLines.frag");
		
        glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		debugQuadDepth_->Use();
		debugQuadDepth_->SetInt("depthMap", 31);
		
		coreShaderProgram->Use();
		coreShaderProgram->SetInt("material.diffuse", 28);
		coreShaderProgram->SetInt("material.specular", 29);
		
		AllocateTextureMemory(pointLightCubeShadowMapFBOcontainer, pointLightCubeShadowMapTextureContainer,
							  GL_TEXTURE_CUBE_MAP, GL_CLAMP_TO_EDGE, 16, "pointLightCubeShadowMapArray", 0);
		AllocateTextureMemory(spotLightFlatShadowMapFBOContainer, spotLightFlatShadowMapTextureContainer,
							  GL_TEXTURE_2D, GL_CLAMP_TO_BORDER, 8, "spotLightFlatShadowMapArray", 16);
		AllocateTextureMemory(directionalLightFlatShadowMapFBOcontainer, directionalLightFlatShadowMapTextureContainer,
							  GL_TEXTURE_2D, GL_CLAMP_TO_BORDER, 4, "directionalLightFlatShadowMapArray", 24);
		
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

		for (unsigned int i = 0; i < VBOcontainer_.size(); ++i)
			pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &VBOcontainer_[i]);
		
		for (unsigned int i = 0; i < VAOcontainer_.size(); ++i)
			pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &VAOcontainer_[i]);
		
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &quadVBO_);
		pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &quadVAO_);
	}
    
	void COpenglRenderer::draw() {
		using namespace GLVM;
		namespace cm = GLVM::ecs::components;

		ecs::ComponentManager* pComponent_Manager = ecs::ComponentManager::GetInstance();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		coreShaderProgram->Use();
		
		core::vector<unsigned int>* pEntityContainerRefDirectionalLight =
			pComponent_Manager->GetEntityContainer<cm::directionalLight>();
		unsigned int appropriateDirectionalLightComponentIndex = 0;
		sampledDirectionalLightEntityIDcontainer.clear();
		mat4 directionalProjectionMatrixLight = ortho(-10.0f, 10.0f, -10.0f, 10.0f,
													  nearPlaneFlatShadowMap, farPlaneFlatShadowMap);
		for ( unsigned int i = 0; i < pEntityContainerRefDirectionalLight->GetSize(); ++i ) {
			unsigned int uiDirectionalLightsEntity = (*pEntityContainerRefDirectionalLight)[i];
			cm::directionalLight* directionalLightComponent = pComponent_Manager->
				GetComponent<cm::directionalLight>(uiDirectionalLightsEntity);

			directionalLightSpaceMatrixContainer[appropriateDirectionalLightComponentIndex] =
				EvaluateFlatShadowMap(directionalLightFlatShadowMapFBOcontainer[i],
									  *directionalLightComponent,directionalProjectionMatrixLight) ;

			sampledDirectionalLightEntityIDcontainer.push_back(i);
			coreShaderProgram->Use();
			coreShaderProgram->SetInt(ConcatIntBetweenTwoStrings("sampledShadowOrdinalNumbers[",
																 appropriateDirectionalLightComponentIndex, "]"), i);
			
			++appropriateDirectionalLightComponentIndex;
		}

		coreShaderProgram->SetInt("sampledDirectionalShadowOrdinalNumbersArraySize",
								  sampledDirectionalLightEntityIDcontainer.size());
		
		core::vector<unsigned int>* pEntityContainerRefSpotLight =
			pComponent_Manager->GetEntityContainer<cm::spotLight>();
		unsigned int appropriateSpotLightComponentIndex = 0;
		sampledSpotLightEntityIDcontainer.clear();
		mat4 spotProjectionMatrixLight = Perspective(Radians(90.0f),
													 (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT,
													 nearPlaneFlatShadowMap, farPlaneFlatShadowMap);
		
		for ( unsigned int i = 0; i < pEntityContainerRefSpotLight->GetSize(); ++i ) {
			unsigned int uiSpotLightsEntity = (*pEntityContainerRefSpotLight)[i];
			cm::spotLight* spotLightComponent = pComponent_Manager->GetComponent<cm::spotLight>(uiSpotLightsEntity);

			spotLightSpaceMatrixContainer[appropriateSpotLightComponentIndex] =
				EvaluateFlatShadowMap(spotLightFlatShadowMapFBOContainer[i],
									  *spotLightComponent,spotProjectionMatrixLight) ;

			sampledSpotLightEntityIDcontainer.push_back(i);
			coreShaderProgram->Use();
			coreShaderProgram->SetInt(ConcatIntBetweenTwoStrings("spotLightFlatShadowMapComponentIndices[",
																 appropriateSpotLightComponentIndex, "]"), i);
			++appropriateSpotLightComponentIndex;
		}

		coreShaderProgram->SetInt("sampledSpotShadowOrdinalNumbersArraySize",
								  sampledSpotLightEntityIDcontainer.size());
		
		core::vector<unsigned int>* pEntityContainerRefView =
			pComponent_Manager->GetEntityContainer<cm::beholder>();
		unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		cm::transform* playerTransformComponent = pComponent_Manager->GetComponent<cm::transform>(uiPlayerEntity);
		core::vector<unsigned int>* pEntityContainerRefPointLight =
			pComponent_Manager->GetEntityContainer<cm::pointLight>();
		unsigned int pointLightComponentContainerSize = pEntityContainerRefPointLight->GetSize();

		sampledPointLightEntityIDcontainer.clear();
		unsigned int appropriatePointLightComponentIndex = 0;
		for ( unsigned int i = 0; i < pointLightComponentContainerSize; ++i ) {
			unsigned int entityID = (*pEntityContainerRefPointLight)[i];
			cm::pointLight* pointLightComponent = pComponent_Manager->GetComponent<cm::pointLight>(entityID);
			float distance = VectorLength(playerTransformComponent->tPosition, pointLightComponent->position);

			if ( distance < 4.5f ) {
				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				sampledPointLightEntityIDcontainer.push_back(i);
				
				EvaluateCubeShadowMap(pointLightCubeShadowMapFBOcontainer[appropriatePointLightComponentIndex],
									  *pointLightComponent);
				
				coreShaderProgram->Use();
				coreShaderProgram->SetInt(ConcatIntBetweenTwoStrings("pointLightCubeShadowMapComponentIndices[",
																	 appropriatePointLightComponentIndex, "]"), i);
				++appropriatePointLightComponentIndex;
			}
		}
		
		coreShaderProgram->SetInt("sampledPointShadowOrdinalNumbersArraySize",
								  sampledPointLightEntityIDcontainer.size());
		
		ComputeDirectionalLight();
		ComputePointLight();
		ComputeSpotLight();

	    EvaluateCoreShader();
		RenderScene(coreShaderProgram);

//		Window.SwapBuffers();

		/*!
		  \brief DEBUG
		  ====================================================================
		  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		  debugQuadDepth_->Use();
		  debugQuadDepth_->SetFloat("nearPlane", nearPlaneFlatShadowMap);
		  debugQuadDepth_->SetFloat("fatPlane", farPlaneFlatShadowMap);
		  glActiveTexture(GL_TEXTURE30);
		  glBindTexture(GL_TEXTURE_2D, spotLightFlatShadowMapFBOContainer[0]);
		  RenderQuad();
		  ====================================================================
        */
	}

	void COpenglRenderer::AllocateTextureMemory(std::vector<unsigned int>& shadowMapFBOcontainer,
												std::vector<unsigned int>& shadowMapTextureContainer,
												GLenum textureTarget, GLint clampType,
												unsigned int lightSourceNumber,
												std::string shadowMapArrayType,
		                                        unsigned int textureUnitBaseIndex) {
		for ( unsigned int i = 0; i < lightSourceNumber; ++i ) {
			shadowMapFBOcontainer.emplace_back();
			shadowMapTextureContainer.emplace_back();
			InitializeShadowMapData(shadowMapFBOcontainer[i], shadowMapTextureContainer[i],
									textureTarget, clampType);
		}
		unsigned int lightCounter = 0;
		int* lightArrayUniformIndices = new int[lightSourceNumber];
		for ( unsigned int i = textureUnitBaseIndex; i < textureUnitBaseIndex + lightSourceNumber; ++i ) {
			lightArrayUniformIndices[lightCounter] = i;
			++lightCounter;
		}

		coreShaderProgram->Use();
		coreShaderProgram->SetInt(shadowMapArrayType, lightSourceNumber, lightArrayUniformIndices);
		delete [] lightArrayUniformIndices;
		lightArrayUniformIndices = nullptr;
	}
	
	void COpenglRenderer::InitializeShadowMapData(unsigned int& fbo_, unsigned int& texture_, GLenum textureTarget_,
												  GLint clampType_) {
				pGLGen_Framebuffers(1, &fbo_);
				glGenTextures(1, &texture_);
				pGLActive_Texture(GL_TEXTURE0);
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

	mat4 COpenglRenderer::EvaluateFlatShadowMap(unsigned int& shadowMapFBO, ecs::components::directionalLight& directionalLightComponent, mat4 projectionMatrixLight) {
		vec3 positionVectorLight  = directionalLightComponent.position;
		vec3 directionVectorLight = directionalLightComponent.direction;
		mat4 viewMatrixLight = LookAtMain(positionVectorLight,
										  directionVectorLight,
										  { 0.0f, 1.0f, 0.0f });
		mat4 lightSpaceMatrix = viewMatrixLight * projectionMatrixLight;
		// Render scene from light's point of view
		flatShadowMapShaderProgram->Use();
		flatShadowMapShaderProgram->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		// Render to depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		pGLBind_Framebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
//		glCullFace(GL_FRONT);
		RenderScene(flatShadowMapShaderProgram);
//		glCullFace(GL_BACK);
		pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);

		return lightSpaceMatrix;
	}

	mat4 COpenglRenderer::EvaluateFlatShadowMap(unsigned int& shadowMapFBO, ecs::components::spotLight& directionalLightComponent, mat4 projectionMatrixLight) {
			vec3 positionVectorLight = directionalLightComponent.position;
			vec3 directionVectorLight = directionalLightComponent.direction;
			mat4 viewMatrixLight = LookAtMain(positionVectorLight,
														 directionVectorLight,
														 { 0.0f, 1.0f, 0.0f });
			mat4 lightSpaceMatrix = viewMatrixLight * projectionMatrixLight;
			// Render scene from light's point of view
			flatShadowMapShaderProgram->Use();
			flatShadowMapShaderProgram->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
			// Render to depth map
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			pGLBind_Framebuffer(GL_FRAMEBUFFER, shadowMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
//		glCullFace(GL_FRONT);
			RenderScene(flatShadowMapShaderProgram);
//		glCullFace(GL_BACK);
			pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);

			return lightSpaceMatrix;
	}
	
	void COpenglRenderer::EvaluateCubeShadowMap(unsigned int& shadowMapFBO, ecs::components::pointLight& pointLightComponent) {
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
				pGLBind_Framebuffer(GL_FRAMEBUFFER, shadowMapFBO);
				glClear(GL_DEPTH_BUFFER_BIT);
				cubeShadowMapShaderProgram->Use();
				for (unsigned int j = 0; j < 6; ++j)
					cubeShadowMapShaderProgram->SetMat4("shadowMatrices[" + std::to_string(j) + "]", cubeShadowMapTransforms[j]);
				cubeShadowMapShaderProgram->SetFloat("farPlane", farPlaneCubeShadowMap);
				cubeShadowMapShaderProgram->SetVec3("lightPosition", positionVectorPointLight);
				RenderScene(cubeShadowMapShaderProgram);
				pGLBind_Framebuffer(GL_FRAMEBUFFER, 0);
	}

	void COpenglRenderer::EvaluateCoreShader() {
		namespace cm = GLVM::ecs::components;
        ecs::ComponentManager* pComponent_Manager = ecs::ComponentManager::GetInstance();
		core::vector<unsigned int>* pEntityContainerRefView =
			pComponent_Manager->GetEntityContainer<cm::beholder>();
		unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		cm::beholder* playerViewComponent = pComponent_Manager->GetComponent<cm::beholder>(uiPlayerEntity);
		cm::transform* playerTransformComponent = pComponent_Manager->GetComponent<cm::transform>(uiPlayerEntity);
		
//		viewPosition = playerViewComponent.Position;
		vec3 viewPosition = playerTransformComponent->tPosition;
		bool reverseNormalsFlag = false;
		
		// Render scene as normal
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		coreShaderProgram->Use();
		ComputeProjectionMatrix(coreShaderProgram);
		ComputeViewMatrix(coreShaderProgram, *playerTransformComponent, *playerViewComponent);
		coreShaderProgram->SetInt("shadows", shadows);
		coreShaderProgram->SetBool("reverseNormals", reverseNormalsFlag);
		coreShaderProgram->SetFloat("farPlane", farPlaneCubeShadowMap);
		coreShaderProgram->SetVec3("viewPosition", viewPosition);

		coreShaderProgram->SetInt("directionalLightSpaceMatrixContainerSize",
								  sampledDirectionalLightEntityIDcontainer.size());
		coreShaderProgram->SetMat4("directionalLightSpaceMatrixContainer",
								   sampledDirectionalLightEntityIDcontainer.size(),
								   directionalLightSpaceMatrixContainer[0]);
		coreShaderProgram->SetInt("spotLightSpaceMatrixContainerSize", sampledSpotLightEntityIDcontainer.size());
		coreShaderProgram->SetMat4("spotLightSpaceMatrixContainer", sampledSpotLightEntityIDcontainer.size(),
								   spotLightSpaceMatrixContainer[0]);
		coreShaderProgram->SetInt("spotLightArraySize", sampledSpotLightEntityIDcontainer.size());

		for ( unsigned int i = 0; i < sampledPointLightEntityIDcontainer.size(); ++i ) {
			pGLActive_Texture( GL_TEXTURE0 + i );
			glBindTexture( GL_TEXTURE_CUBE_MAP, pointLightCubeShadowMapTextureContainer[i] );
		}

		for ( unsigned int i = 0; i < sampledSpotLightEntityIDcontainer.size(); ++i ) {
			pGLActive_Texture( GL_TEXTURE16 + i );
			glBindTexture( GL_TEXTURE_2D, spotLightFlatShadowMapTextureContainer[i] );
		}

		for ( unsigned int i = 0; i < sampledDirectionalLightEntityIDcontainer.size(); ++i ) {
			pGLActive_Texture( GL_TEXTURE24 + i );
			glBindTexture( GL_TEXTURE_2D, directionalLightFlatShadowMapTextureContainer[i] );
		}
	}

	void COpenglRenderer::EvaluateFlatDebugShader() {
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		debugQuadDepth_->Use();
		debugQuadDepth_->SetFloat("nearPlane", nearPlaneFlatShadowMap);
		debugQuadDepth_->SetFloat("farPlane", farPlaneFlatShadowMap);
		pGLActive_Texture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, directionalLightFlatShadowMapTextureContainer[0]);
	}
	
	void COpenglRenderer::ComputeDirectionalLight() {
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
		core::vector<unsigned int>* pEntityContainerRefDirectionalLight =
			pComponent_Manager->GetEntityContainer<cm::directionalLight>();
		unsigned int directionalLightComponentContainerSize = pEntityContainerRefDirectionalLight->GetSize();

		coreShaderProgram->SetInt("directionalLightsArraySize", directionalLightComponentContainerSize);
		for(unsigned int x = 0; x < directionalLightComponentContainerSize; ++x) {
			unsigned int uiDirectionalLightEntity = (*pEntityContainerRefDirectionalLight)[x];
			cm::directionalLight* directionalLightComponent = pComponent_Manager->GetComponent<cm::directionalLight>(uiDirectionalLightEntity);
			std::string leftString = "directionalLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
									   directionalLightComponent->position);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].direction"),
										directionalLightComponent->direction);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
									   directionalLightComponent->ambient);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
									   directionalLightComponent->diffuse); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
									   directionalLightComponent->specular);

		}
	}

	void COpenglRenderer::ComputePointLight() {
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
		core::vector<unsigned int>* pEntityContainerRefPointLight =
			pComponent_Manager->GetEntityContainer<cm::pointLight>();
		unsigned int pointLightComponentContainerSize = pEntityContainerRefPointLight->GetSize();
		coreShaderProgram->SetInt("pointLightsArraySize", pointLightComponentContainerSize);
		for(unsigned int x = 0; x < pointLightComponentContainerSize; ++x) {
			unsigned int uiPointLightEntity = (*pEntityContainerRefPointLight)[x];
			cm::pointLight* pointLightComponent = pComponent_Manager->GetComponent<cm::pointLight>(uiPointLightEntity);
			
			// core::vector<unsigned int>* pEntityContainerRefView = ecs::GetEntityContainer<ecs::beholder>(*pComponent_Manager);
			// unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
			// ecs::transform& playerTransformComponent = pComponent_Manager->GetComponent<ecs::transform>(uiPlayerEntity);

			std::string leftString = "pointLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
									   pointLightComponent->position);
			// coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
			// 						   playerTransformComponent.tPosition);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
									   pointLightComponent->ambient);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
									   pointLightComponent->diffuse); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
									   pointLightComponent->specular);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].constant"),
										pointLightComponent->constant);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].linear"),
										pointLightComponent->linear);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].quadratic"),
										pointLightComponent->quadratic);
		}
	}

	void COpenglRenderer::ComputeSpotLight() {
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
		core::vector<unsigned int>* pEntityContainerRefSpotLight =
			pComponent_Manager->GetEntityContainer<cm::spotLight>();
		unsigned int spotLightComponentContainerSize = pEntityContainerRefSpotLight->GetSize();
		coreShaderProgram->SetInt("spotLightsArraySize", spotLightComponentContainerSize);
		for(unsigned int x = 0; x < spotLightComponentContainerSize; ++x) {
			unsigned int uiSpotLightEntity = (*pEntityContainerRefSpotLight)[x];
			cm::spotLight* spotLightComponent = pComponent_Manager->GetComponent<cm::spotLight>(uiSpotLightEntity);
			std::string leftString = "spotLights[";
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].position"),
									   spotLightComponent->position);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].direction"),
									   spotLightComponent->direction);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].cutOff"),
										std::cos(Radians(spotLightComponent->cutOff)));
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].outerCutOff"),
										std::cos(Radians(spotLightComponent->outerCutOff)));
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].ambient"),
									   spotLightComponent->ambient);
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].diffuse"),
									   spotLightComponent->diffuse); // darken diffuse light a bit
			coreShaderProgram->SetVec3(ConcatIntBetweenTwoStrings(leftString, x, "].specular"),
									   spotLightComponent->specular);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].constant"),
										spotLightComponent->constant);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].linear"),
										spotLightComponent->linear);
			coreShaderProgram->SetFloat(ConcatIntBetweenTwoStrings(leftString, x, "].quadratic"),
										spotLightComponent->quadratic);
		}
	}
	
	void COpenglRenderer::RenderScene(Shader* shaderProgram_) {
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
		mat4 modelMatrix(1.0f);

		Raycasting();
		// RaycastringDebug();
		// coreShaderProgram->Use();

		ecs::TextureManager* textureManager = ecs::TextureManager::GetInstance();
		std::vector<ecs::Texture>& texture_load_data_ = textureManager->GetTextureVector();
		
		if ( frameAccumulator >= frames[currentFrame] ) {
			++currentFrame;
			if ( currentFrame == frames.GetSize() ) {
				currentFrame = 0;
				frameAccumulator = 0.0f;
			}

			mat4 jointMatricesData[4];
			jointMatricesData[0] = jointMatricesPerMesh[0][0][currentFrame];
			jointMatricesData[1] = jointMatricesPerMesh[0][1][currentFrame];
			jointMatricesData[2] = jointMatricesPerMesh[0][2][currentFrame];
			jointMatricesData[3] = jointMatricesPerMesh[0][3][currentFrame];
			// for ( int i = 0; i < 4; ++i )
			// 	std::cout << jointMatricesData[i] << std::endl;
			coreShaderProgram->SetMat4("jointMatrices", 4, jointMatricesData[0]);
		} else {
			mat4 jointMatricesData[4];
			jointMatricesData[0] = jointMatricesPerMesh[0][0][currentFrame];
			jointMatricesData[1] = jointMatricesPerMesh[0][1][currentFrame];
			jointMatricesData[2] = jointMatricesPerMesh[0][2][currentFrame];
			jointMatricesData[3] = jointMatricesPerMesh[0][3][currentFrame];
			// for ( int i = 0; i < 4; ++i )
			// 	std::cout << jointMatricesData[i] << std::endl;
			coreShaderProgram->SetMat4("jointMatrices", 4, jointMatricesData[0]);
		}

		for(unsigned int i = 0; i < texture_load_data_.size(); ++i) {
			for (unsigned int j = 0; j < texture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = texture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];

				cm::vertex* vertexComponent = pComponent_Manager->GetComponent<cm::vertex>(uiEntity_refTexture);
				unsigned int uiVertexId = 0;
				if ( vertexComponent != nullptr )
					uiVertexId = vertexComponent->vkVertexId_;

				cm::material* material = pComponent_Manager->GetComponent<cm::material>(uiEntity_refTexture);
				unsigned int diffuseTextureID  = 0;
				unsigned int specularTextureID = 0;
				if ( material != nullptr ) {
					diffuseTextureID  = material->diffuseTextureID_;
					specularTextureID = material->specularTextureID_;
				}

				cm::transform* transformComponent = pComponent_Manager->GetComponent<cm::transform>(uiEntity_refTexture);
				if ( transformComponent != nullptr )
					modelMatrix = SetModelMatrix(*transformComponent);
				
				shaderProgram_->SetMat4("modelMatrix", modelMatrix);
				pGLActive_Texture(GL_TEXTURE28);
				glBindTexture(GL_TEXTURE_2D, texture_load_data_[diffuseTextureID].iTexture_);
				pGLActive_Texture(GL_TEXTURE29);
		 		glBindTexture(GL_TEXTURE_2D, texture_load_data_[specularTextureID].iTexture_);
				pGLBind_Vertex_Array(VAOcontainer_[uiVertexId]);
				cm::material* materialComponent = pComponent_Manager->GetComponent<cm::material>(uiEntity_refTexture);
				shaderProgram_->SetFloat("material.shininess", materialComponent->shininess);
				shaderProgram_->SetVec3("material.ambient",  materialComponent->ambient[0], materialComponent->ambient[1], materialComponent->ambient[2]);
				// coreShaderProgram->SetVec3("material.diffuse",  materialComponent.diffuse[0], materialComponent.diffuse[1], materialComponent.diffuse[2]); // darken diffuse light a bit
//				coreShaderProgram->SetVec3("material.specular", materialComponent.specular[0], materialComponent.specular[1], materialComponent.specular[2]);
				glDrawElements(GL_TRIANGLES, aIndices_[uiVertexId].size(), GL_UNSIGNED_INT, 0);

			}
		}

		for(unsigned int i = 0; i < hudTexture_load_data_.size(); ++i)
			for (unsigned int j = 0; j < hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_.size(); ++j) {
				unsigned int uiEntity_refTexture = hudTexture_load_data_[i].entitiesOwnsThisTypeOfTexture_[j];
                unsigned int uiVertexId = pComponent_Manager->GetComponent<cm::vertex>(uiEntity_refTexture)->vkVertexId_;
				modelMatrix = SetModelMatrix(*pComponent_Manager->GetComponent<cm::transform>(uiEntity_refTexture));
				shaderProgram_->SetMat4("modelMatrix", modelMatrix);
				pGLActive_Texture(GL_TEXTURE30);
				pGLBind_Vertex_Array(VAOcontainer_[uiVertexId]);
				glDrawElements(GL_TRIANGLES, aIndices_[uiVertexId].size(), GL_UNSIGNED_INT, 0);
			}
	}

	void COpenglRenderer::Raycasting() {
		namespace cm = GLVM::ecs::components;
		ecs::ComponentManager* componentManager  = ecs::ComponentManager::GetInstance();
		core::vector<Entity> linkedEntities      = componentManager->collectUniqueLinkedEntities<cm::projectile,
																								 cm::transform,
																								 cm::material,
																								 cm::vertex,
																								 cm::collider>();
		
		core::vector<Entity> otherLinkedEntities = componentManager->collectUniqueLinkedEntities<cm::material,
																								 cm::collider,
																								 cm::vertex,
																								 cm::transform>();

		// std::cout << "linged entities size " << linkedEntities.GetSize() << std::endl;
		// linkedEntities.Print();

		// std::cout << "other linged entities size :" << otherLinkedEntities.GetSize() << std::endl;
		// otherLinkedEntities.Print();
		
		unsigned int linkedEntitiesVectorSize      = linkedEntities.GetSize();
		unsigned int otherLinkedEntitiesVectorSize = otherLinkedEntities.GetSize();

        for(unsigned int x = 0; x < linkedEntitiesVectorSize; ++x) {
          unsigned int uiEntity_refProjectile = linkedEntities[x];
          cm::transform* rTransformProjectile = componentManager->GetComponent<cm::transform>(uiEntity_refProjectile);
		  float rayLength = 1.0f;
		  vec3 ray        = rTransformProjectile->tForward * rayLength;

			for(unsigned int j = 0; j < otherLinkedEntitiesVectorSize; ++j) {
				unsigned int entityOther = otherLinkedEntities[j];
				cm::transform* transformOther = componentManager->GetComponent<cm::transform>(entityOther);
				float otherHalfScale = transformOther->fScale * 0.5f;
				float min = 0.0f;
				float max = 1.0f;

				for ( int dimension = 0; dimension < 3; ++dimension ) {
					float axis_invariant = 1.0f / ray[dimension];
					float box_min = transformOther->tPosition[dimension] - otherHalfScale;
					float box_max = transformOther->tPosition[dimension] + otherHalfScale;
					
					float delta1  = (box_min - rTransformProjectile->tPosition[dimension]) * axis_invariant;
					float delta2  = (box_max - rTransformProjectile->tPosition[dimension]) * axis_invariant;

					min = Max(min, Min(delta1, delta2));
					max = Min(max, Max(delta1, delta2));
					
					if ( max < min )
						break;
				}

				if ( max > min ) {
					// std::cout << "TEST 2" << std::endl;
					std::cout << "projectile entity: " << uiEntity_refProjectile << std::endl;
					std::cout << "other entity: " << entityOther << std::endl;
					GLVM::ecs::TextureManager* textureSystem = GLVM::ecs::TextureManager::GetInstance();
					ecs::EntityManager* entityManager       = GLVM::ecs::EntityManager::GetInstance();
					cm::material* textureProjectile = componentManager->GetComponent<cm::material>(uiEntity_refProjectile);
					textureSystem->UnbindTexture(*textureProjectile, uiEntity_refProjectile);
					entityManager->RemoveEntity(uiEntity_refProjectile, componentManager);
					/// TODO: There is a big quastion is this decrement have sence.
//					--linkedEntitiesVectorSize;  
 				}
			}
		}
	}

	
	void COpenglRenderer::RaycastingDebug() {
		/// TODO: This code for debug purpouses only
		// float plane[] = {
		// 	-0.3f, -0.3f, -0.5f, 0.3f, 0.5f, 0.7f,
		// 	0.3f, -0.3f, -0.5f, 0.3f, 0.5f, 0.7f,
		// 	0.3f,  0.3f, -0.5f, 0.3f, 0.5f, 0.7f,
		// 	0.3f,  0.3f, -0.5f, 0.3f, 0.5f, 0.7f,
		// 	-0.3f,  0.3f, -0.5f, 0.3f, 0.5f, 0.7f,
		// 	-0.3f, -0.3f, -0.5f, 0.3f, 0.5f, 0.7f
		// };


		debugLines->Use();
		
		mat4 planeModelMatrix(1.0);
		planeModelMatrix[0][0] = 5.0;
		planeModelMatrix[1][1] = 5.0;
		planeModelMatrix[2][2] = 5.0;
		planeModelMatrix[3][3] = 1.0;
		planeModelMatrix[3][0] = 3.0;
		planeModelMatrix[3][1] = 5.0;
		planeModelMatrix[3][2] = 3.0;

		// for ( int i = 0; i < 4; ++i )
		// 	for ( int j = 0; j < 4; ++j)
		// 		std::cout << planeModelMatrix[i][j] << std::endl;
		
        // Matrix<float, 4> planeModelMatrix(1.0);
        // planeModelMatrix[0][0] = 0.05;
        // planeModelMatrix[1][1] = 0.05;
        // planeModelMatrix[2][2] = 0.05;

		namespace cm = GLVM::ecs::components;
        ecs::ComponentManager* componentManager = ecs::ComponentManager::GetInstance();
		core::vector<unsigned int>* pEntityContainerRefView =
			componentManager->GetEntityContainer<cm::beholder>();
		unsigned int uiPlayerEntity = (*pEntityContainerRefView)[0];
		cm::beholder* playerViewComponent = componentManager->GetComponent<cm::beholder>(uiPlayerEntity);
		cm::transform* playerTransformComponent = componentManager->GetComponent<cm::transform>(uiPlayerEntity);
		
//		viewPosition = playerViewComponent.Position;
		
//		debugLines->SetMat4("modelMatrix", planeModelMatrix);
		unsigned int location = pGLGet_Uniform_Location(debugLines->iID, "modelMatrix");
		pGLUniform_Matrix4fv(location, NUMBER_OF_MATRICES, GL_FALSE, &planeModelMatrix[0][0]);
		ComputeProjectionMatrix(debugLines);
		ComputeViewMatrix(debugLines, *playerTransformComponent, *playerViewComponent);
		pGLGen_Vertex_Arrays(1, &vaoPlane);
		pGLGen_Buffers(1, &vboPlane);
		pGLBind_Vertex_Array(vaoPlane);
		pGLBind_Buffer(GL_ARRAY_BUFFER, vboPlane);
		pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

		pGLVertex_Attrib_Pointer(LAYOUT_0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)VERTEX_OFFSET);
		pGLEnable_Vertex_Attrib_Array(LAYOUT_0);
		pGLVertex_Attrib_Pointer(LAYOUT_1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(LAYOUT_1);
		
		// pGLBind_Buffer(GL_ARRAY_BUFFER, 0);
		// pGLBind_Vertex_Array(0);

        glClear(GL_DEPTH_BUFFER_BIT);
		
//		pGLBind_Buffer(GL_ARRAY_BUFFER, vboLines);
//		pGLBind_Vertex_Array(vaoPlane);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		pGLBind_Buffer(GL_ARRAY_BUFFER, 0); 

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        pGLBind_Vertex_Array(0);         

		mat4 linesModelMatrix(1.0);
		linesModelMatrix[0][0] = 1.0;
		linesModelMatrix[1][1] = 1.0;
		linesModelMatrix[2][2] = 1.0;
		linesModelMatrix[3][3] = 1.0;
		linesModelMatrix[3][0] = 0.0;
		linesModelMatrix[3][1] = 0.0;
		linesModelMatrix[3][2] = 0.0;

		unsigned int locationLines = pGLGet_Uniform_Location(debugLines->iID, "modelMatrix");
		pGLUniform_Matrix4fv(locationLines, NUMBER_OF_MATRICES, GL_FALSE, &linesModelMatrix[0][0]);

		float lines[36] = {
			0.0f, 5.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 5.0f, -5.5f,
		    0.0f, 1.0f, 0.0f,
			
			0.0f, 5.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 10.0f, 0.0f,
		    0.0f, 1.0f, 0.0f,
			
			0.0f, 5.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			5.0f, 5.0f, 0.0f,
		    0.0f, 1.0f, 0.0f
		};
		
		pGLGen_Vertex_Arrays(1, &vaoLines);
		pGLGen_Buffers(1, &vboLines);
		pGLBind_Vertex_Array(vaoLines);
		pGLBind_Buffer(GL_ARRAY_BUFFER, vboLines);
		pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_DYNAMIC_DRAW);

		pGLVertex_Attrib_Pointer(LAYOUT_0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)VERTEX_OFFSET);
		pGLEnable_Vertex_Attrib_Array(LAYOUT_0);
		pGLVertex_Attrib_Pointer(LAYOUT_1, VERTEX_SIZE, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(LAYOUT_1);

		pGLBind_Vertex_Array(vaoLines);
		glDrawArrays(GL_LINES, 0, 6);	
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

		for ( int i = 0; i < _aVertices.size(); ++i)
			std::cout << _aVertices[i] << std::endl;
		
		pGLBind_Buffer(GL_ARRAY_BUFFER, iVbo_);
        pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(float) * _aVertices.size(), _aVertices.data(), GL_DYNAMIC_DRAW);

        pGLBind_Buffer(GL_ELEMENT_ARRAY_BUFFER, iEbo_);
        pGLBuffer_Data(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _aIndices.size(), _aIndices.data(), GL_STATIC_DRAW);
        
        // pGLVertex_Attrib_Pointer(LAYOUT_0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)VERTEX_OFFSET);
        // pGLEnable_Vertex_Attrib_Array(LAYOUT_0);
		// pGLVertex_Attrib_Pointer(LAYOUT_1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		// pGLEnable_Vertex_Attrib_Array(LAYOUT_1);
		// pGLVertex_Attrib_Pointer(2, TEXTURE_SIZE, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		// pGLEnable_Vertex_Attrib_Array(2);
		
		pGLVertex_Attrib_Pointer(LAYOUT_0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)VERTEX_OFFSET);
        pGLEnable_Vertex_Attrib_Array(LAYOUT_0);
		pGLVertex_Attrib_Pointer(LAYOUT_1, 3, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(3 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(LAYOUT_1);
		pGLVertex_Attrib_Pointer(2, TEXTURE_SIZE, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(6 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(2);
		pGLVertex_Attrib_Pointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(3);
		pGLVertex_Attrib_Pointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));
		pGLEnable_Vertex_Attrib_Array(4);

		VBOcontainer_.push_back(iVbo_);
		VAOcontainer_.push_back(iVao_);
		EBOcontainer_.push_back(iEbo_);
	}
	
    void COpenglRenderer::loadWavefrontObj() {
        for (unsigned int m = 0; m < pathsArray_.size(); ++m) {
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
            for (unsigned int i = 0; i < faceVerticesSize; ++i)
                for (unsigned int j = 0; j < 3; ++j) {
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

	void COpenglRenderer::LoadGLTF() {
		for (unsigned int m = 0; m < pathsGLTF.GetSize(); ++m) {
			
			Core::CJsonParser parser;
			parser.ReadFile(pathsGLTF[m]);
			parser.Parse();

			// const Core::JsonValue* binaryPath = parser.Search("uri");
			// std::string binary_path;
			// if ( binaryPath != nullptr ) {
			// 	binary_path = *binaryPath->value.string;
			// 	std::cout << binary_path << std::endl;
			// }

			Core::JsonValue* gltf = parser.GetRoot();
			std::string binary_path = *(*gltf)["buffers"][0]["uri"].value.string;
			std::cout << binary_path << std::endl;
			int full_byte_size = (*gltf)["buffers"][0]["byteLength"].value.iNumber;;
//			std::cout << binary_path << std::endl;
			std::ifstream in_stream;
			in_stream.open("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/" + binary_path, std::ios::binary);
 			char* buffer = new char[full_byte_size];
			in_stream.read(buffer, full_byte_size);
			in_stream.close();

			// for ( int i = 0; i < full_byte_size; i += 4 )
			// 	std::cout << reinterpret_cast<float &>(buffer[i]) << std::endl;

			int indices_index = (*gltf)["meshes"][0]["primitives"][0]["indices"].value.iNumber;
			int indices_buffer_view_index = (*gltf)["accessors"][indices_index]["bufferView"].value.iNumber;
			int indices_byte_length = (*gltf)["bufferViews"][indices_buffer_view_index]["byteLength"].value.iNumber;
			int indices_byte_offset = (*gltf)["bufferViews"][indices_buffer_view_index]["byteOffset"].value.iNumber;

//			std::cout << "length: " << indices_byte_length << " offset: " << indices_byte_offset << std::endl;
			
			core::vector<unsigned int> indices;
			for ( int i = indices_byte_offset; i < indices_byte_offset + indices_byte_length; i += 2 )
				indices.Push(reinterpret_cast<unsigned short &>(buffer[i]));

//			baseIndices.Push(indices);
			
			int vertices_position_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["POSITION"].value.iNumber;
			int vertices_buffer_view_index = (*gltf)["accessors"][vertices_position_index]["bufferView"].value.iNumber;
			int vertices_byte_length = (*gltf)["bufferViews"][vertices_buffer_view_index]["byteLength"].value.iNumber;
			int vertices_byte_offset = (*gltf)["bufferViews"][vertices_buffer_view_index]["byteOffset"].value.iNumber;

//			std::cout << "length: " << vertices_byte_length << " offset: " << vertices_byte_offset << std::endl;
			
			core::vector<float> vertices_position;
			for ( int i = vertices_byte_offset; i < vertices_byte_offset + vertices_byte_length; i += 4 )
				vertices_position.Push(reinterpret_cast<float &>(buffer[i]));

			int texture_coordinates_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["TEXCOORD_0"].value.iNumber;
			int texture_buffer_view_index = (*gltf)["accessors"][texture_coordinates_index]["bufferView"].value.iNumber;
			int texture_byte_length = (*gltf)["bufferViews"][texture_buffer_view_index]["byteLength"].value.iNumber;
			int texture_byte_offset = (*gltf)["bufferViews"][texture_buffer_view_index]["byteOffset"].value.iNumber;

//			std::cout << "length: " << texture_byte_length << " offset: " << texture_byte_offset << std::endl;

			core::vector<float> texture_coordinates;
			for ( int i = texture_byte_offset; i < texture_byte_offset + texture_byte_length; i += 4 )
				texture_coordinates.Push(reinterpret_cast<float &>(buffer[i]));

			int normals_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["NORMAL"].value.iNumber;
			int normals_buffer_view_index = (*gltf)["accessors"][normals_index]["bufferView"].value.iNumber;
			int normals_byte_length = (*gltf)["bufferViews"][normals_buffer_view_index]["byteLength"].value.iNumber;
			int normals_byte_offset = (*gltf)["bufferViews"][normals_buffer_view_index]["byteOffset"].value.iNumber;

//			std::cout << "length: " << normals_byte_length << " offset: " << normals_byte_offset << std::endl;

			core::vector<float> normals;
			for ( int i = normals_byte_offset; i < normals_byte_offset + normals_byte_length; i += 4 )
				normals.Push(reinterpret_cast<float &>(buffer[i]));

			core::vector<Core::JsonValue> skins = parser.Search("skins");
//			core::vector<Core::JsonValue> accessors = parser.Search("accessors");
			Core::JsonValue joints;
			core::vector<mat4> globalTransformJointNode;
			core::vector<mat4> inverseBindMatrixSet;
			core::vector<core::vector<mat4>> jointMatrices;
			// core::vector<Vector<int ,4>> jointIndicesLocalContainer;
			// core::vector<vec4> weightsLocalContainer;
			core::vector<float> weightsContainer;
			core::vector<int> jointsIndices;
			
			if ( skins.GetSize() > 0 ) {
				joints = (*gltf)["skins"][0]["joints"];
//				joints = skins[0]["joints"];
				// unsigned int inverseBindMatricesAccessorIndex = skins[0]["inverseBindMatrices"].value.iNumber;
				// Core::JsonValue accessor = accessors[inverseBindMatricesAccessorIndex];
				// unsigned int bufferView = accessor["bufferView"].value.iNumber;
				// std::cout << bufferView << std::endl;
				// for( unsigned int i = 0; i < joints->GetSize(); ++i ) {
				// 	std::cout << (*joints)[i].value.iNumber << std::endl;
				// }

				Core::JsonValue nodes = (*gltf)["nodes"];
				for ( unsigned int i = 0; i < joints.value.array->GetSize(); ++i ) {
					unsigned int index = (*joints.value.array)[i].value.iNumber;
//					std::cout << "index: " << index << std::endl;
					Core::JsonValue node = nodes[index];
					Quaternion rotationQuaternion;
					mat4 rotation(1.0f);
					mat4 scale(1.0f);
					mat4 translation(1.0f);

					if ( node.value.object->Contain("rotation") ) {
						Core::JsonValue array = (*node.value.object)["rotation"];
						for ( unsigned int i = 0; i < array.value.array->GetSize(); ++i ) {
							switch(i) {
							case 0:
								if ( array[i].isInterger() )
									rotationQuaternion.x = array[i].value.iNumber;
								else if ( array[i].isFloat() )
									rotationQuaternion.x = array[i].value.fNumber;
							case 1:
								if ( array[i].isInterger() )
									rotationQuaternion.y = array[i].value.iNumber;
								else if ( array[i].isFloat() )
									rotationQuaternion.y = array[i].value.fNumber;
							case 2:
								if ( array[i].isInterger() )
									rotationQuaternion.z = array[i].value.iNumber;
								else if ( array[i].isFloat() )
									rotationQuaternion.z = array[i].value.fNumber;
							case 3:
								if ( array[i].isInterger() )
									rotationQuaternion.w = array[i].value.iNumber;
								else if ( array[i].isFloat() )
									rotationQuaternion.w = array[i].value.fNumber;
							}
						}

						// rotationQuaternion.x = (*node.value.object)["rotation"][0].value.fNumber;
						// rotationQuaternion.y = (*node.value.object)["rotation"][1].value.fNumber;
						// rotationQuaternion.z = (*node.value.object)["rotation"][2].value.fNumber;
						// rotationQuaternion.w = (*node.value.object)["rotation"][3].value.fNumber;

						rotation = rotateQuaternion<float, 4>(rotationQuaternion);
					}

//					std::cout << rotationQuaternion << std::endl;
//					std::cout << rotation << std::endl;
					
					if ( node.value.object->Contain("scale") ) {
						Core::JsonValue array = (*node.value.object)["scale"];
						for ( unsigned int i = 0; i < array.value.array->GetSize(); ++i ) {
							if ( array[i].isInterger() )
								scale[i][i] = array[i].value.iNumber;
							else if ( array[i].isFloat() )
								scale[i][i] = array[i].value.fNumber;
						}
						// scale[0][0] = (*node.value.object)["scale"][0].value.fNumber;
						// scale[1][1] = (*node.value.object)["scale"][1].value.fNumber;
						// scale[2][2] = (*node.value.object)["scale"][2].value.fNumber;
						// std::cout << "govno 1: " << (*node.value.object)["scale"][0].value.fNumber << std::endl;;
						// std::cout << "govno 2: " << (*node.value.object)["scale"][1].value.fNumber << std::endl;;
						// std::cout << "govno 3: " << (*node.value.object)["scale"][2].value.fNumber << std::endl;;

					}

//					std::cout << scale << std::endl;
					
					if ( node.value.object->Contain("translation") ) {
						Core::JsonValue array = (*node.value.object)["translation"];
						for ( unsigned int i = 0; i < array.value.array->GetSize(); ++i ) {
							if ( array[i].isInterger() )
								translation[3][i] = array[i].value.iNumber;
							else if ( array[i].isFloat() )
								translation[3][i] = array[i].value.fNumber;
						}


					}

					mat4 model = translation * scale * rotation;
					globalTransformJointNode.Push(model);
//					std::cout << translation << std::endl;
					// if ( node.value.object->Contain("rotation") )
					// 	rotationQuaternion.x = (*node.value.object)["rotation"][0].value.fNumber;
				}

//				std::cout << globalTransformJointNode[1] << std::endl;

				unsigned int inverseBindMatricesIndex = (*gltf)["skins"][0]["inverseBindMatrices"].value.iNumber;
				unsigned int bufferView = (*gltf)["accessors"][inverseBindMatricesIndex]["bufferView"].value.iNumber;
				unsigned int byteLengthInverseBindMatrices = (*gltf)["bufferViews"][bufferView]["byteLength"].value.iNumber;
				unsigned int byteOffsetInverseBindMatrices = (*gltf)["bufferViews"][bufferView]["byteOffset"].value.iNumber;

				core::vector<float> inverseBindMatricesData;
				for ( unsigned int i = byteOffsetInverseBindMatrices; i < byteOffsetInverseBindMatrices + byteLengthInverseBindMatrices; i += 4 )
					inverseBindMatricesData.Push(reinterpret_cast<float &>(buffer[i]));

//				inverseBindMatricesData.Print();
//				std::cout << inverseBindMatricesData.GetSize() << std::endl;
				
//				core::vector<mat4> jointMatriciesLocalContainer;

				for ( unsigned int n = 0; n < joints.value.array->GetSize(); ++n ) {
					mat4 inverseBindMatrix(0.0f);
					for ( unsigned int g = 0; g < 4; ++g )
						for ( unsigned int j = 0; j < 4; ++j )
							inverseBindMatrix[g][j] = inverseBindMatricesData[g * 4 + j];

					inverseBindMatrixSet.Push(inverseBindMatrix);
					// mat4 jointMatrix = globalTransformJointNode[n] * inverseBindMatrix;
					// jointMatriciesLocalContainer.Push(jointMatrix);
				}

//				jointMatrices.Push(jointMatriciesLocalContainer);
				
				unsigned int joints_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["JOINTS_0"].value.iNumber;
				unsigned int joints_buffer_view_index = (*gltf)["accessors"][joints_index]["bufferView"].value.iNumber;
				unsigned int joints_byte_length = (*gltf)["bufferViews"][joints_buffer_view_index]["byteLength"].value.iNumber;
				unsigned int joints_byte_offset = (*gltf)["bufferViews"][joints_buffer_view_index]["byteOffset"].value.iNumber;

				for ( unsigned int i = joints_byte_offset; i < joints_byte_offset + joints_byte_length; ++i )
					jointsIndices.Push(reinterpret_cast<char &>(buffer[i]));

//				jointsIndices.Print();

				jointIndicesToShader = jointsIndices;
				
				// for ( unsigned int v = 0; v < indices.GetSize(); ++v ) {
				// 	unsigned int offset = 4;
				// 	unsigned int index = indices[v] * offset;
				// 	if ( index + 3 < jointsIndices.GetSize() ) {
				// 		Vector<int, 4> indices = { jointsIndices[index],
				// 			jointsIndices[index + 1],
				// 			jointsIndices[index + 2],
				// 			jointsIndices[index + 3]};
						
				// 		jointIndicesLocalContainer.Push(indices);
				// 	}
				// }
				
//				jointIndicesPerVertex.Push(jointIndicesLocalContainer);

				unsigned int weights_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["WEIGHTS_0"].value.iNumber;
				unsigned int weights_buffer_view_index = (*gltf)["accessors"][weights_index]["bufferView"].value.iNumber;
				unsigned int weights_byte_length = (*gltf)["bufferViews"][weights_buffer_view_index]["byteLength"].value.iNumber;
				unsigned int weights_byte_offset = (*gltf)["bufferViews"][weights_buffer_view_index]["byteOffset"].value.iNumber;

				for ( unsigned int i = weights_byte_offset; i < weights_byte_offset + weights_byte_length; i += 4 )
					weightsContainer.Push(reinterpret_cast<float &>(buffer[i]));

//				weightsContainer.Print();

				weightsToShader = weightsContainer;
				
				// for ( unsigned int v = 0; v < indices.GetSize(); ++v ) {
				// 	unsigned int offset = 4;
				// 	unsigned int index = indices[v] * offset;
				// 	if ( index + 3 < weightsContainer.GetSize() ) {
				// 		vec4 weights = { weightsContainer[index],
				// 			weightsContainer[index + 1],
				// 			weightsContainer[index + 2],
				// 			weightsContainer[index + 3]};

				// 		weightsLocalContainer.Push(weights);
				// 	}
				// }
//				std::cout << weightsLocalContainer.GetSize() << std::endl;
//				weightsPerVertex.Push(weightsLocalContainer);
			}

			core::vector<Core::JsonValue> animations = parser.Search("animations");

			if ( animations.GetSize() > 0 ) {
				core::vector<Core::JsonValue> samplerIndices;
				core::vector<Core::JsonValue> targetNodes;
				core::vector<Core::JsonValue> targetPaths;
				Core::JsonValue channels = (*gltf)["animations"][0]["channels"];
				for ( unsigned int i = 0; i < channels.value.array->GetSize(); ++i )
					samplerIndices.Push(channels[i]["sampler"]);

				// for ( unsigned int i = 0; i < samplerIndices.GetSize(); ++i )
				// 	std::cout << samplerIndices[i].value.iNumber << std::endl;;
				
				for ( unsigned int i = 0; i < channels.value.array->GetSize(); ++i )
					targetNodes.Push(channels[i]["target"]["node"]);

				for ( unsigned int i = 0; i < channels.value.array->GetSize(); ++i )
					targetPaths.Push(channels[i]["target"]["path"]);

				core::vector<unsigned int> translationSamplerIndices;
				core::vector<unsigned int> rotationSamplerIndices;
				core::vector<unsigned int> scaleSamplerIndices;
				for ( unsigned int i = 0; i < samplerIndices.GetSize(); ++i ) {
					if ( *targetPaths[i].value.string == "translation" ) {
						translationSamplerIndices.Push(samplerIndices[i].value.iNumber);
					} else if ( *targetPaths[i].value.string == "rotation" ) {
						rotationSamplerIndices.Push(samplerIndices[i].value.iNumber);
					} else if ( *targetPaths[i].value.string == "scale" ) {
						scaleSamplerIndices.Push(samplerIndices[i].value.iNumber);
					}
				}

				Core::JsonValue samplers = (*gltf)["animations"][0]["samplers"];
				
				core::vector<unsigned int> translationInputs;
				core::vector<unsigned int> translationOutputs;
				
				for ( unsigned int i = 0; i < translationSamplerIndices.GetSize(); ++i)
					translationInputs.Push(samplers[translationSamplerIndices[i]]["input"].value.iNumber);

				for ( unsigned int i = 0; i < translationSamplerIndices.GetSize(); ++i)
					translationOutputs.Push(samplers[translationSamplerIndices[i]]["output"].value.iNumber);

				core::vector<core::vector<float>> frameInputsTranslation;
				for ( unsigned int i = 0; i < translationInputs.GetSize(); ++i) {
					unsigned int frameBufferViewIndex =
						(*gltf)["accessors"][translationInputs[i]]["bufferView"].value.iNumber;
					unsigned int frameByteLength      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int frameByteOffset      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

//					temp.Print();
					frameInputsTranslation.Push(temp);
				}

				core::vector<core::vector<float>> translations;
				for ( unsigned int i = 0; i < translationOutputs.GetSize(); ++i) {
					// std::cout << i << std::endl;
					// std::cout << "index: " << translationOutputs[i] << std::endl;
					unsigned int outputBufferViewIndex =
						(*gltf)["accessors"][translationOutputs[i]]["bufferView"].value.iNumber;
					unsigned int outputByteLength      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int outputByteOffset      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

//					std::cout << "length: " << outputByteLength << " offset: " << outputByteOffset << std::endl;
					
					core::vector<float> temp;
					for ( unsigned int i = outputByteOffset; i < outputByteOffset + outputByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

//					temp.Print();
					translations.Push(temp);
				}

				core::vector<unsigned int> rotationInputs;
				core::vector<unsigned int> rotationOutputs;
				
				for ( unsigned int i = 0; i < rotationSamplerIndices.GetSize(); ++i)
					rotationInputs.Push(samplers[rotationSamplerIndices[i]]["input"].value.iNumber);

				for ( unsigned int i = 0; i < rotationSamplerIndices.GetSize(); ++i)
					rotationOutputs.Push(samplers[rotationSamplerIndices[i]]["output"].value.iNumber);

				core::vector<core::vector<float>> frameInputsRotation;
				for ( unsigned int i = 0; i < rotationInputs.GetSize(); ++i) {
					unsigned int frameBufferViewIndex =
						(*gltf)["accessors"][rotationInputs[i]]["bufferView"].value.iNumber;
					unsigned int frameByteLength      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int frameByteOffset      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

//					temp.Print();
					frameInputsRotation.Push(temp);
				}

				core::vector<core::vector<float>> rotations;
				for ( unsigned int i = 0; i < rotationOutputs.GetSize(); ++i) {
					unsigned int outputBufferViewIndex =
						(*gltf)["accessors"][rotationOutputs[i]]["bufferView"].value.iNumber;
					unsigned int outputByteLength      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int outputByteOffset      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = outputByteOffset; i < outputByteOffset + outputByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

//					temp.Print();
					rotations.Push(temp);
				}

				core::vector<unsigned int> scaleInputs;
				core::vector<unsigned int> scaleOutputs;
				
				for ( unsigned int i = 0; i < scaleSamplerIndices.GetSize(); ++i)
					scaleInputs.Push(samplers[scaleSamplerIndices[i]]["input"].value.iNumber);

				for ( unsigned int i = 0; i < translationSamplerIndices.GetSize(); ++i)
					scaleOutputs.Push(samplers[scaleSamplerIndices[i]]["output"].value.iNumber);

				core::vector<core::vector<float>> frameInputsScale;
				for ( unsigned int i = 0; i < scaleInputs.GetSize(); ++i) {
					unsigned int frameBufferViewIndex =
						(*gltf)["accessors"][scaleInputs[i]]["bufferView"].value.iNumber;
					unsigned int frameByteLength      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int frameByteOffset      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

//					temp.Print();
					frameInputsScale.Push(temp);
				}

				core::vector<core::vector<float>> scales;
				for ( unsigned int i = 0; i < scaleOutputs.GetSize(); ++i) {
					unsigned int outputBufferViewIndex =
						(*gltf)["accessors"][scaleOutputs[i]]["bufferView"].value.iNumber;
					unsigned int outputByteLength      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int outputByteOffset      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = outputByteOffset; i < outputByteOffset + outputByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

//					temp.Print();
					scales.Push(temp);
				}

				frames = frameInputsTranslation[0];

				for ( unsigned int j = 0; j < translations.GetSize(); ++j ) {
					core::vector<float> boneAllFrameTranslations = translations[j];
					// std::cout << "index: " << j << std::endl;
					// boneAllFrameTranslations.Print();
					core::vector<float> boneAllFrameRotations    = rotations[j];
					core::vector<float> boneAllFrameScales       = scales[j];
					core::vector<mat4>  globalAllFrameNodeMatrix;
//					std::cout << "node #: " << j << std::endl;
					for ( unsigned int i = 0; i < frameInputsTranslation[0].GetSize(); ++i ) {
						mat4 frameTranslation(1.0f);
						mat4 frameScale(1.0f);
						for ( unsigned int q = 0; q < 3; ++q ) {
							frameTranslation[3][q] = boneAllFrameTranslations[i * 3 + q];
							frameScale[q][q]       = boneAllFrameScales[i * 3 + q];
						}
//						std::cout << frameTranslation << std::endl;

						Quaternion frameRotationQuaternion;
						mat4 frameRotation(1.0f);
						frameRotationQuaternion.x = boneAllFrameRotations[i * 4];
						frameRotationQuaternion.y = boneAllFrameRotations[i * 4 + 1];
						frameRotationQuaternion.z = boneAllFrameRotations[i * 4 + 2];
						frameRotationQuaternion.w = boneAllFrameRotations[i * 4 + 3];
						frameRotation = rotateQuaternion<float, 4>(frameRotationQuaternion);
//						std::cout << frameRotation << std::endl;

						mat4 globalTransformNodeMatrix = frameTranslation * frameScale * frameRotation;
						globalAllFrameNodeMatrix.Push(globalTransformNodeMatrix * inverseBindMatrixSet[j]);
//						globalTransformJointNode.Push(jointFrameMatrix);

						// scale[0][0] = (*node.value.object)["scale"][0].value.fNumber;
						// scale[1][1] = (*node.value.object)["scale"][1].value.fNumber;
						// scale[2][2] = (*node.value.object)["scale"][2].value.fNumber;
						// std::cout << "govno 1: " << (*node.value.object)["scale"][0].value.fNumber << std::endl;;
						// std::cout << "govno 2: " << (*node.value.object)["scale"][1].value.fNumber << std::endl;;
						// std::cout << "govno 3: " << (*node.value.object)["scale"][2].value.fNumber << std::endl;;

					}

					jointMatrices.Push(globalAllFrameNodeMatrix);
				}
//				std::cout << jointMatrices.GetSize() << std::endl;
				unsigned int maximumJoints     = 4;
				unsigned int unitMatricesSize = maximumJoints - jointMatrices.GetSize();

				if ( unitMatricesSize ) {
					for ( unsigned int i = 0; i < unitMatricesSize; ++i) {
						core::vector<mat4>  globalAllFrameNodeMatrix;
						for ( unsigned int j = 0; j < frameInputsTranslation[0].GetSize(); ++j ) {
							mat4 unitMatrix(1.0f);
							globalAllFrameNodeMatrix.Push(unitMatrix);
						}

						jointMatrices.Push(globalAllFrameNodeMatrix);
					}
				}

				// Quaternion rotationQuaternion;
				// if ( node.value.object->Contain("translation") ) {
				// 	Core::JsonValue array = (*node.value.object)["translation"];
				// 	for ( unsigned int i = 0; i < array.value.array->GetSize(); ++i ) {
				// 		if ( array[i].isInterger() )
				// 			translation[3][i] = array[i].value.iNumber;
				// 		else if ( array[i].isFloat() )
				// 			translation[3][i] = array[i].value.fNumber;
				// 	}


				// }					
//			}
				
			// core::vector<Core::JsonValue> inputs;
			// core::vector<Core::JsonValue> outputs;
				
				// Core::JsonValue samplers = (*gltf)["animations"][0]["samplers"];
				// for ( unsigned int i = 0; i < samplers.value.array->GetSize(); ++i)
				// 	inputs.Push(samplers[i]["input"].value.iNumber);

				// for ( unsigned int i = 0; i < samplers.value.array->GetSize(); ++i)
				// 	outputs.Push(samplers[i]["output"].value.iNumber);

				// unsigned int frameBufferViewIndex = (*gltf)["accessors"][7]["bufferView"].value.iNumber;
				// unsigned int frameByteLength      = (*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
				// unsigned int frameByteOffset      = (*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

				// core::vector<float> frames;
				// for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 )
				// 	frames.Push(reinterpret_cast<float &>(buffer[i]));
				
				
				
				// for ( unsigned int i = 0; i < frames.GetSize(); ++i)
				// 	std::cout << frames[i] << std::endl;
//				std::string* target = (*gltf)["animations"][0]["channels"][0]["target"]["path"].value.string;

				// std::cout << std::endl;
				
				// core::vector<unsigned int> bufferViewsIndices;
				// for ( unsigned int i = 0; i < samplers.GetSize(); ++i ) {
				// 	bufferViewsIndices.Push((*gltf)["animations"][0]["samplers"][samplers[i].value.iNumber]["input"].value.iNumber);
				// 	std::cout << bufferViewsIndices[i] << std::endl;
				// }
				
			} else {
				unsigned int maximumJoints = 4;
				for ( unsigned int i = 0; i < maximumJoints; ++i) {
					core::vector<mat4>  globalAllFrameNodeMatrix;
					for ( unsigned int j = 0; j < 1; ++j ) {      ///< If we dont have animations then we have 1 frame
						mat4 unitMatrix(1.0f);
						globalAllFrameNodeMatrix.Push(unitMatrix);
					}

					jointMatrices.Push(globalAllFrameNodeMatrix);
				}
			}
//			std::cout << jointMatrices[0][0] << std::endl;
			jointMatricesPerMesh.Push(jointMatrices);

			// bool scaleFlag = (*gltf)["nodes"][0].value.object->Contain("scale");
			// mat3 scaleTransform(1.0f);

//			std::cout << "scale flag: " << scaleFlag << std::endl;
			
// 			if ( scaleFlag ) {
// 				core::vector<Core::JsonValue> scale = *(*gltf)["nodes"][0]["scale"].value.array;
// 				for ( int i = 0; i < 3; ++i ) {
// 					if ( scale[i].type == Core::JSON_FLOAT_NUMBER )
// 						scaleTransform[i][i] = scale[i].value.fNumber;

// 					if ( scale[i].type == Core::JSON_INTEGER_NUMBER )
// 						scaleTransform[i][i] = (float)scale[i].value.iNumber;
// 				}

// //				std::cout << "scale: " << scaleTransform << std::endl;
// 			}
			// indices.Print();
			// std::cout << "size: " << indices.GetSize() << std::endl;

			// std::cout << "vertex size: " << vertices_position.GetSize() << std::endl;
			aVertexes_.emplace_back();
			aIndices_.emplace_back();

			baseIndices = indices;
			
			for ( unsigned int i = 0; i < indices.GetSize(); ++i ) {
				aIndices_[m].push_back(i);

//				std::cout << indices[i] - 1 << std::endl;
				unsigned int index = indices[i] * 3;
				if ( index + 2 < vertices_position.GetSize() ) {
					vec3 position = { vertices_position[index],
						vertices_position[index + 1],
						vertices_position[index + 2] };

					// if ( scaleFlag )
					// 	position = position * scaleTransform;
					
					aVertexes_[m].push_back(position[0]);
					aVertexes_[m].push_back(position[1]);
					aVertexes_[m].push_back(position[2]);
				}

				if ( index + 2 < normals.GetSize() ) {
					vec3 normal = { normals[index],
						normals[index + 1],
						normals[index + 2] };

					// if ( scaleFlag )
					// 	normal = normal * scaleTransform;
					
					aVertexes_[m].push_back(normal[0]);
					aVertexes_[m].push_back(normal[1]);
					aVertexes_[m].push_back(normal[2]);
				}

				index = indices[i] * 2;
				if ( index + 1 < texture_coordinates.GetSize() ) {
					aVertexes_[m].push_back(texture_coordinates[index]);
					aVertexes_[m].push_back(texture_coordinates[index + 1]);
				}

				index = indices[i] * 4;
				if ( index + 3 < jointsIndices.GetSize() ) {
					aVertexes_[m].push_back(jointsIndices[index]);
					aVertexes_[m].push_back(jointsIndices[index + 1]);
					aVertexes_[m].push_back(jointsIndices[index + 2]);
					aVertexes_[m].push_back(jointsIndices[index + 3]);
				}

				if ( index + 3 < weightsContainer.GetSize() ) {
					aVertexes_[m].push_back(weightsContainer[index]);
					aVertexes_[m].push_back(weightsContainer[index + 1]);
					aVertexes_[m].push_back(weightsContainer[index + 2]);
					aVertexes_[m].push_back(weightsContainer[index + 3]);
				}
			}

			SetVertices(aIndices_[m], aVertexes_[m]);
			
			// core::vector<Core::JsonValue> vector = parser.Search("name");
			// std::cout << "size: " << vector.GetSize() << std::endl;
			// for ( unsigned int i = 0; i < vector.GetSize(); ++i )
			// 	std::cout << *vector[i].value.string << std::endl;
			
			// int byteLength = 840;
			// int byteOffset = 768;


			// FILE* ptr;

			// ptr = fopen(("/home/cyberdemon/cyberdemon_code/GLVMEngine/gltf/" + binary_path).c_str(), "rb");
			// fread(buffer, byteLength, 1, ptr);

			// for(int i = 0; i < 840; i += 4)
			// 	printf("%d ", buffer[i]);
			
		}
		
	}

	void COpenglRenderer::EnlargeFrameAccumulator(float value) {
		frameAccumulator += value;
	}
	
	mat4 COpenglRenderer::SetModelMatrix(ecs::components::transform& transformComponent_)
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

		// std::cout << "yaw: " << transformComponent_.yaw << std::endl;
		// std::cout << "pitch: " << transformComponent_.pitch << std::endl;

//		transformComponent_.rotate += delta;

		float sinPitch = std::sin(Radians(-transformComponent_.pitch / 2));
		float cosPitch = std::cos(Radians(-transformComponent_.pitch / 2));
		float sinYaw = std::sin(Radians(-(transformComponent_.yaw + 90.0f)  / 2));
		float cosYaw = std::cos(Radians(-(transformComponent_.yaw + 90.0f)  / 2));
		
		Quaternion pitchQuat;
		Quaternion yawQuat;
		pitchQuat.w = cosPitch;
		pitchQuat.x = 0.0f;
		pitchQuat.y = 0.0f;
		pitchQuat.z = sinPitch;

		yawQuat.w = cosYaw;
		yawQuat.x = 0.0f;
		yawQuat.y = sinYaw;
		yawQuat.z = 0.0f;

		Quaternion result;
//		result = eulerToQuaternion(0.0f, Radians(pitch) , Radians(fYaw));
//		result = multiplyQuaternion(yawQuat, Quaternion{ .w = 0.0f, .x = 1.0f, .y = 0.0f, .z = 0.0f });

//		std::cout << result << std::endl;
		// yawQuat = multiplyQuaternion(multiplyQuaternion(yawQuat, Quaternion{ .w = 0.0f, .x = 0.0f,
		// 				.y = 0.0f, .z = 1.0f }), inverseQuaternion(yawQuat));

		// pitchQuat = multiplyQuaternion(multiplyQuaternion(pitchQuat, Quaternion{ .w = 0.0f, .x = 1.0f,
		// 				.y = 0.0f, .z = 0.0f }), inverseQuaternion(pitchQuat));

		result = multiplyQuaternion(yawQuat, pitchQuat);
		// mat4 yaw   = Rotate<float, 4>(Vector<float, 3>(0.0f, 1.0f, 0.0f), -transformComponent_.yaw);
		// mat4 pitch = Rotate<float, 4>(Vector<float, 3>(0.0f, 0.0f, 1.0f), transformComponent_.pitch);

//		result = eulerToQuaternion(0.0f, transformComponent_.pitch, transformComponent_.yaw);
		
		// std::cout << "yaw: " << yaw << std::endl;
		// std::cout << "pitch: " << pitch << std::endl;
		rotationMatrix = rotateQuaternion<float, 4>(result);
//		std::cout << "rotation: " << rotationMatrix << std::endl;
		// std::cout << "x: " << transformComponent_.tPosition[0] << std::endl;
		// std::cout << "y: " << transformComponent_.tPosition[1] << std::endl;
		// std::cout << "z: " << transformComponent_.tPosition[2] << std::endl;

//		std::cout << rotationMatrix << std::endl;
		
        modelMatrix = scalingMatrix * translationMatrix;

		// rotationMatrix[0][0] = 30.0f;
		// rotationMatrix[1][1] = 30.0f;
		// rotationMatrix[2][2] = 30.0f;
		
		return rotationMatrix * modelMatrix;
	}

    void COpenglRenderer::SetViewMatrix(mat4 _viewMatrix) {
        unsigned int uniformLocationViewWorld = pGLGet_Uniform_Location(coreShaderProgram->iID, "viewMatrix");
        pGLUniform_Matrix4fv(uniformLocationViewWorld, NUMBER_OF_MATRICES, GL_FALSE, &_viewMatrix[0][0]);
    }

    void COpenglRenderer::SetProjectionMatrix(mat4 _projectionMatrix) {
        unsigned int uniformLocationProjectionWorld = pGLGet_Uniform_Location(coreShaderProgram->iID, "projectionMatrix");
		pGLUniform_Matrix4fv(uniformLocationProjectionWorld, NUMBER_OF_MATRICES, GL_FALSE, &_projectionMatrix[0][0]);
    }
    
    void COpenglRenderer::SetTextureData(std::vector<ecs::Texture>& _texture_data, std::vector<ecs::Texture>& _hud_texture_data) {
		texture_load_data = _texture_data;
		hudTexture_load_data_ = _hud_texture_data;
	}
	
    void COpenglRenderer::SetMeshData(std::vector<const char*> _pathsArray, core::vector<const char*> pathsGLTF_) {
		for (unsigned int i = 0; i < _pathsArray.size(); ++i)
            pathsArray_.push_back(_pathsArray[i]);

		for (unsigned int i = 0; i < pathsGLTF_.GetSize(); ++i)
			pathsGLTF.Push(pathsGLTF_[i]);
	}
    
    void COpenglRenderer::run() {
//		loadWavefrontObj();
		LoadGLTF();
	}

	void COpenglRenderer::ComputeViewMatrix(Shader* shaderProgram, ecs::components::transform& player, ecs::components::beholder& beholder)
    {
		// static int viewCount = 0;
		// std::cout << "Count: " << viewCount << std::endl;
		// ++viewCount;
		
        Matrix<float, 4> viewMatrix(1.0f);
        const float kSensitivity = 0.1f;

		// std::cout << "x offset: " << g_eEvent.mousePointerPosition.offset_X << std::endl;
		// std::cout << "y offset: " << g_eEvent.mousePointerPosition.offset_Y << std::endl;
		
        fYaw = g_eEvent.mousePointerPosition.offset_X;
        pitch = g_eEvent.mousePointerPosition.offset_Y;
        fYaw *= kSensitivity;
        pitch *= kSensitivity;

        g_eEvent.mousePointerPosition.pitch = pitch;
        g_eEvent.mousePointerPosition.yaw = fYaw;
        
        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;

		// std::cout << "Pitch in radians: " << Radians(pitch) << std::endl;
		// std::cout << "Yaw in radians: " << Radians(fYaw) << std::endl;
		
		// std::cout << "cos yaw: " << std::cos(Radians(fYaw)) << std::endl;
		// std::cout << "sin yaw: " << std::sin(Radians(fYaw)) << std::endl;
		// std::cout << "cos pitch: " << std::cos(Radians(pitch)) << std::endl;
		// std::cout << "sin pitch: " << std::sin(Radians(pitch)) << std::endl;
		
		vec3 forward;
		float sinPitch = std::sin(Radians(pitch / 2));
		float cosPitch = std::cos(Radians(pitch / 2));
		float sinYaw = std::sin(Radians(-fYaw / 2));
		float cosYaw = std::cos(Radians(-fYaw / 2));
		
		Quaternion pitchQuat;
		Quaternion yawQuat;
		pitchQuat.w = cosPitch;
		pitchQuat.x = sinPitch;
		pitchQuat.y = 0.0f;
		pitchQuat.z = 0.0f;

		yawQuat.w = cosYaw;
		yawQuat.x = 0.0f;
		yawQuat.y = sinYaw;
		yawQuat.z = 0.0f;
		/// We have dot product here to compute projection to axes
        // forward[0] = std::cos(Radians(fYaw)) * std::cos(Radians(pitch));    ///< Projection to x axis
		// forward[1] = std::sin(Radians(pitch));                              ///< Projection to y axis
        // forward[2] = std::sin(Radians(fYaw)) * std::cos(Radians(pitch));    ///< Projection to z axis

		// quaternion1 = MultiplyQuaternion(Quaternion{ .real = std::cos(Radians(pitch)), .imaginary = vec3{ 1.0f, 0.0f, 0.0f } * std::sin(Radians(pitch)) },
		// 								Quaternion{ .real = 0.0f, .imaginary = Normalize(vec3{ 0.0f, 0.0f, -1.0f}) });
		// quaternion2 = MultiplyQuaternion(Quaternion{ .real = -std::cos(Radians(fYaw)), .imaginary = vec3{ 0.0f, 1.0f, 0.0f } * std::sin(Radians(fYaw)) },
		// 								 Quaternion{ .real = 0.0f, .imaginary = Normalize(vec3{ -1.0f, 0.0f, 0.0f })});
		
		Quaternion result;
//		result = eulerToQuaternion(0.0f, Radians(pitch) , Radians(fYaw));
//		result = multiplyQuaternion(yawQuat, Quaternion{ .w = 0.0f, .x = 1.0f, .y = 0.0f, .z = 0.0f });
		result = multiplyQuaternion(yawQuat, pitchQuat);

		result = multiplyQuaternion(multiplyQuaternion(result, Quaternion{ .w = 0.0f, .x = 0.0f,
					.y = 0.0f, .z = -1.0f }), inverseQuaternion(result));

//		std::cout << "x: " << result.x << " y: " << result.y << " z: " << result.z << std::endl;
		
		// result = multiplyQuaternion(pitchQuat, Quaternion{ .w = 0.0f, .x = 0.0f, .y = 0.0f, .z = 1.0f });
		// result = multiplyQuaternion(result, yawQuat);

		// result = multiplyQuaternion(multiplyQuaternion(result, Quaternion{ .w = 0.0f, .x = 1.0f,
		// 			.y = 0.0f, .z = 0.0f }), inverseQuaternion(result));

		
		// quaternion1.real = std::cos(Radians(pitch));
		// quaternion1.imaginary = Cross(beholder.forward, vec3{ 0.0f, 1.0f, 0.0f }) * std::sin(Radians(pitch));
		// quaternion2.real = std::cos(Radians(-fYaw));
		// quaternion2.imaginary = vec3{ 0.0f, 1.0f, 0.0f } * std::sin(Radians(-fYaw));

		// quaternion1.real = std::cos(Radians(pitch));
		// quaternion1.imaginary = vec3{ std::sin(pitch), 0.0f, 0.0f };
		// quaternion2.real = std::cos(Radians(fYaw));
		// quaternion2.imaginary = vec3{ 0.0f, std::sin(fYaw), 0.0f };
		
		// quaternion3 = MultiplyQuaternion(quaternion1, quaternion2);
		// quaternion3 = MultiplyQuaternion(quaternion3, Quaternion{ .real = 0.0f, .imaginary = Normalize(vec3{ 0.0f, 1.0f, 0.0f })});
//		quaternion3 = MultiplyQuaternion(quaternion3, quaternion1);
//		forward = quaternion3.imaginary;
		forward[0] = result.x;
		forward[1] = result.y;
		forward[2] = result.z;
        beholder.forward = Normalize(forward);

//		std::cout << beholder.forward << std::endl;
		
		// std::cout << "Opengl" << std::endl;
		// std::cout << "x: " << beholder.forward[0] << " y: " << beholder.forward[1] << " z: " << beholder.forward[2] << std::endl;
		
        viewMatrix = LookAtMain(player.tPosition,
								player.tPosition + beholder.forward,
								beholder.up);

 		// _view_Component.Position[0] = _Player.tPosition[0];
		// _view_Component.Position[1] = _Player.tPosition[1];
		// _view_Component.Position[2] = _Player.tPosition[2];

		// for ( int i = 0; i < 4; ++i )
		// 	for ( int j = 0; j < 4; ++j )
		// 			std::cout <<  "View: " << viewMatrix[i][j] << std::endl;
		
		shaderProgram->SetMat4("viewMatrix", viewMatrix);
    }

	void COpenglRenderer::ComputeProjectionMatrix(Shader* shaderProgram) {
		// static int projectionCount = 0;
		// std::cout << "Count: " << projectionCount << std::endl;
		// ++projectionCount;

		mat4 tProjection_Matrix = Perspective(Radians(90.0f), (float)1920 / (float)1080, 0.1f, 100.0f);

 		// for ( int i = 0; i < 4; ++i )
		// 	for ( int j = 0; j < 4; ++j )
		// 			std::cout <<  "Projection: " << tProjection_Matrix[i][j] << std::endl;
		
		shaderProgram->SetMat4("projectionMatrix", tProjection_Matrix);
	}
}
