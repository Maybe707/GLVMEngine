#ifndef OPENGL
#define OPENGL

#include "ComponentManager.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Constants.hpp"
#include "Event.hpp"
#include "GLPointer.h"
#include "IRenderer.hpp"
#include "ISystem.hpp"
#include "Texture.hpp"
#include "TextureManager.hpp"
#include "VectorContainer.hpp"
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

namespace GLVM::Core {
	class COpenglRenderer : public IRenderer {
	public:
#ifdef __linux__
		CWindowX Window;
#endif

#ifdef _WIN32
		CWindowWin Window;
#endif

		const unsigned int SCREEN_WIDTH  = 1920;
		const unsigned int SCREEN_HEIGHT = 1080;
		Shader* coreShaderProgram;
		Shader* flatShadowMapShaderProgram;
		Shader* cubeShadowMapShaderProgram;
		Shader* debugQuadDepth_;
		GLuint quadVAO_;
		GLuint quadVBO_;
		GLuint planeVAO_;
		GLuint planeVBO_;
		unsigned int flatShadowMapFBO;
		unsigned int cubeShadowMapFBO;
		unsigned int flatShadowMapTexture;
		unsigned int cubeShadowMapTexture;
		const unsigned int SHADOW_WIDTH  = 1024;
		const unsigned int SHADOW_HEIGHT = 1024;
		float fYaw   = -90.0f;
        float fPitch = 0.0f;
		unsigned int woodTexture;
		vec3 viewPosition{1.0f};
		mat4 lightSpaceMatrix{1.0f};
		vec3 positionVectorPointLight{0.0f, 3.3f, 2.7f};
		float nearPlaneFlatShadowMap = 1.0f;
		float farPlaneFlatShadowMap = 25.0f;
		float nearPlaneCubeShadowMap = 1.0f;
		float farPlaneCubeShadowMap  = 25.0f;
		bool shadows = true;
		float timeAccumulator = 0.0f;
		bool timeFlag = false;
		std::vector<ECS::CTexture> texture_load_data_;
        std::vector<ECS::CTexture> hudTexture_load_data_;
		std::vector<const char*> pathsArray_;
		std::vector<std::vector<float>> aVertexes_;
		std::vector<std::vector<unsigned int>> aIndices_;
		std::vector<GLuint> VBOcontainer_;
		std::vector<GLuint> VAOcontainer_;
		std::vector<GLuint> EBOcontainer_;

		COpenglRenderer();
		~COpenglRenderer();

		void draw() override;
		void ComputeDirectionalLight();
		void ComputePointLight();
		void ComputeSpotLight();
		void EvaluateFlatShadowMap();
		void EvaluateCubeShadowMap();
		void EvaluateCoreShader();
		void EvaluateFlatDebugShader();
		void RenderScene(Shader* shaderProgram_);
		void RenderQuad();
		void SetVertices(std::vector<unsigned int>& _aIndices,
						 std::vector<float>& _aVertices);
		void loadWavefrontObj() override;
		void SetTextureData(std::vector<ECS::CTexture>& _texture_data, std::vector<ECS::CTexture>& _hud_texture_data) override;
		void SetMeshData(std::vector<const char*> _pathsArray) override;
		void run() override;
		mat4 SetModelMatrix(ECS::STransformComponent& transformComponent_);
		void SetViewMatrix(mat4 _viewMatrix) override;
		void SetProjectionMatrix(mat4 _projectionMatrix) override;
		void ComputeViewMatrix(Shader* shaderProgram, ECS::STransformComponent& _Player, ECS::CViewComponent& _view_Component);
		void ComputeProjectionMatrix(Shader* shaderProgram);
		void renderScene(const Shader& shader);
		void renderCube();
		unsigned int loadTexture(char const * path);
	};
} // namespace GLVM::Core

#endif
