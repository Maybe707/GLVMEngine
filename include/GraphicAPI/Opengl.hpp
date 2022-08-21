#ifndef OPENGL
#define OPENGL

#include "GLPointer.h"
#include <GL/gl.h>
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

/*! \class Renderer.
    \brief Render all game objects.

    Take a game object to render in DrawSprite method.
*/

namespace GLVM::ECS
{    
    class COpenglRenderer : public ISystem
    {
	public:
	    GLuint iVbo_;
		GLuint iVao_;
		Shader* _Shader_Program;

        COpenglRenderer();
		~COpenglRenderer();

		void Update() override;
        void SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component);
    };
}
    
#endif
