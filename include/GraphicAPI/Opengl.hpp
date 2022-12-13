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

		GLuint iVbo_;
		Shader* _Shader_Program;

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
		void SetVertices(std::vector<unsigned int>& _aIndices,
						 std::vector<float>& _aVertices);
		void loadWavefrontObj() override;
		void SetTextureData(std::vector<ECS::CTexture>& _texture_data, std::vector<ECS::CTexture>& _hud_texture_data) override;
		void SetMeshData(std::vector<const char*> _pathsArray) override;
		void run() override;
		void LoadTextureData(GLVM::ECS::CTexture& _Texture);
		void SetModelMatrix(Shader *_Shader_Program,
							ECS::STransformComponent& _transform_Component);
		void SetViewMatrix(mat4 _viewMatrix) override;
		void SetProjectionMatrix(mat4 _projectionMatrix) override;
	};
} // namespace GLVM::Core

#endif
