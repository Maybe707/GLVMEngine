#ifndef OPENGL
#define OPENGL

#include "ComponentManager.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/SpotLightComponent.hpp"
#include "Constants.hpp"
#include "Event.hpp"
#include "GLPointer.h"
#include "IRenderer.hpp"
#include "ISystem.hpp"
#include "Texture.hpp"
#include "TextureManager.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include "WavefrontObjParser.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include "ShaderProgram.hpp"
#include "ToString.hpp"

#ifdef __linux__
#include "UnixApi/WindowX.hpp"
#endif

#ifdef _WIN32
#include "WinApi/WindowWin.hpp"
#endif

/*! \class Renderer.
  \brief Render all game objects.

  Take a game object to render in DrawSprite method.
*/

namespace GLVM::core {
	class COpenglRenderer : public IRenderer {
	public:
#ifdef __linux__
		CWindowX Window;
#endif

#ifdef _WIN32
		CWindowWin Window;
#endif

//		unsigned int appropriateLightComponentIndex = 0;
		const unsigned int SCREEN_WIDTH  = 1920;
		const unsigned int SCREEN_HEIGHT = 1080;
		const unsigned int SHADOW_WIDTH  = 1024;
		const unsigned int SHADOW_HEIGHT = 1024;
		Shader* coreShaderProgram;
		Shader* flatShadowMapShaderProgram;
		Shader* cubeShadowMapShaderProgram;
		Shader* debugQuadDepth_;
		Shader* debugLines;                            ///< For debug only
		GLuint quadVAO_;
		GLuint quadVBO_;

		GLuint vboPlane, vaoPlane;
		float plane[18] = {
			-10.0f, -10.0f, -10.0f,
			10.0f, -10.0f, -10.0f,
			10.0f, 10.0f, -10.0f,
			10.0f, 10.0f, -10.0f,
			-10.0f, 10.0f, -10.0f,
			-10.0f, -10.0f, -10.0f
		};

		GLuint vboLines, vaoLines;
		float lines[36] = {
			-10.0f, 10.0f, 8.0f, 0.0f, 0.0f, 0.0f,
			10.0f, 10.0f, 8.0f, 0.0f, 0.0f, 0.0f, 
			-10.0f, -10.0f, 8.0f, 0.0f, 0.0f, 0.0f,
			10.0f, 10.0f, 8.0f, 0.0f, 0.0f, 0.0f,
			10.0f, 10.0f, 8.0f, 0.0f, 0.0f, 0.0f, 
			10.0f, -10.0f, 8.0f, 0.0f, 0.0f, 0.0f
		};

		
		/// Directional light
		std::vector<unsigned int> directionalLightFlatShadowMapFBOcontainer;
		std::vector<unsigned int> directionalLightFlatShadowMapTextureContainer;
		std::vector<unsigned int> sampledDirectionalLightEntityIDcontainer; ///< Sampled depend on distance from light source to object entity IDs for poit light shadow map.

		/// Point light
		std::vector<unsigned int> pointLightCubeShadowMapFBOcontainer;
		std::vector<unsigned int> pointLightCubeShadowMapTextureContainer;
		std::vector<unsigned int> sampledPointLightEntityIDcontainer; ///< Sampled depend on distance from light source to object entity IDs for poit light shadow map.

		/// Spot light
		std::vector<unsigned int> spotLightFlatShadowMapTextureContainer;
		std::vector<unsigned int> spotLightFlatShadowMapFBOContainer;
		std::vector<unsigned int> sampledSpotLightEntityIDcontainer;
		float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; ///< Border color for fix shadow issue in flat shadow map in long range.
		float fYaw   = -90.0f;
        float pitch = 0.0f;
		mat4 spotLightSpaceMatrixContainer[8];
		mat4 directionalLightSpaceMatrixContainer[4];
		float nearPlaneFlatShadowMap = 1.0f;
		float farPlaneFlatShadowMap = 25.0f;
		float nearPlaneCubeShadowMap = 1.0f;
		float farPlaneCubeShadowMap  = 25.0f;
		bool shadows = true;
		std::vector<ecs::Texture> texture_load_data_;
        std::vector<ecs::Texture> hudTexture_load_data_;
		std::vector<const char*> pathsArray_; ///< Paths to files with vertecis data.
		std::vector<std::vector<float>> aVertexes_;
		std::vector<std::vector<unsigned int>> aIndices_;
		std::vector<GLuint> VBOcontainer_;
		std::vector<GLuint> VAOcontainer_;
		std::vector<GLuint> EBOcontainer_;
		unsigned int directionalLightUniformLocations[5];
		unsigned int pointLightUniformLocations[7];
		unsigned int spotLightUniformLocations[10];

		COpenglRenderer();
		~COpenglRenderer();

		void draw() override;
		void AllocateTextureMemory(std::vector<unsigned int>& shadowMapFBOcontainer,
								   std::vector<unsigned int>& shadowMapTextureContainer,
								   GLenum textureTarget, GLint clampType,
								   unsigned int lightSourceNumber,
			                       std::string shadowMapArrayType,
								   unsigned int textureUnitBaseIndex);
		void InitializeShadowMapData(unsigned int& fbo_, unsigned int& texture_, GLenum textureTarget_,
									 GLint clampType_);
		void AllocateTexture(GLenum textureTarget_, GLint clampType_ );
		void ComputeDirectionalLight();
		void ComputePointLight();
		void ComputeSpotLight();
		mat4 EvaluateFlatShadowMap(unsigned int& shadowMapFBO, ecs::components::directionalLight& directionalLightComponent,
			                       mat4 projectionMatrixLight);
		mat4 EvaluateFlatShadowMap(unsigned int& shadowMapFBO, ecs::components::spotLight& directionalLightComponent,
			                       mat4 projectionMatrixLight);
		void EvaluateCubeShadowMap(unsigned int& shadowMapFBO, ecs::components::pointLight& pointLightComponent);
		void EvaluateCoreShader();
		void EvaluateFlatDebugShader();
		void RenderScene(Shader* shaderProgram_);
		void RenderQuad();
		void SetVertices(std::vector<unsigned int>& _aIndices,
						 std::vector<float>& _aVertices);
		void loadWavefrontObj() override;
		void SetTextureData(std::vector<ecs::Texture>& _texture_data, std::vector<ecs::Texture>& _hud_texture_data) override;
		void SetMeshData(std::vector<const char*> _pathsArray) override;
		void run() override;
		mat4 SetModelMatrix(ecs::components::transform& transformComponent_);
		void SetViewMatrix(mat4 _viewMatrix) override;
		void SetProjectionMatrix(mat4 _projectionMatrix) override;
		void ComputeViewMatrix(Shader* shaderProgram, ecs::components::transform& player, ecs::components::beholder& beholder);
		void ComputeProjectionMatrix(Shader* shaderProgram);
		void renderScene(const Shader& shader);
		void renderCube();
	};
} // namespace GLVM::core

#endif
