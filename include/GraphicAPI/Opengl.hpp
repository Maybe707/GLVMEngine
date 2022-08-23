#ifndef OPENGL
#define OPENGL

#include "GLPointer.h"
#include <GL/gl.h>
#include "IRenderer.hpp"
#include "ISystem.hpp"
#include "VectorContainer.hpp"
#include "Components/VertexComponent.hpp"
#include "ComponentManager.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <GL/glext.h>
#include "Components/AnimationMoveComponent.hpp"
#include "VertexMath.hpp"
#include "Event.hpp"
#include "Components/ViewComponent.hpp"
#include "Constants.hpp"

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

namespace GLVM::Core
{    
    class COpenglRenderer : public IRenderer
    {
	public:
#ifdef __linux__
        CWindowX Window;
#endif

#ifdef _WIN32
        CWindowWin Window;
#endif
        
	    GLuint iVbo_;
		GLuint iVao_;
		Shader* _Shader_Program;
        Shader* GUI_Shader_Program_;

        COpenglRenderer();
		~COpenglRenderer();

		void draw();
        void LoadTextureData(GLVM::ECS::CTextureComponent& _Texture);
        void SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component);
    };
}
    
#endif
