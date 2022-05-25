#ifndef RENDERER
#define RENDERER

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
    class CRenderSystem : public ISystem
    {
	public:
	    GLuint iVbo_;
		GLuint iVao_;

		static const unsigned int u_iRange = 4;
        Matrix<float, 4> tProjection_Matrix{1.0f};
		Shader* _Shader_Program;
        
        ///< Mouse parameters.
        float fYaw = -90.0f;
        float fPitch = 0.0f;
        float fLast_X = 1920.0f / 2.0f;
        float fLast_Y = 1080.0f / 2.0f;
        bool bFirst_Mouse = true;

        CRenderSystem();
		~CRenderSystem();

		void Update() override;
		void SetProjectionMatrix(Shader* _Shader_Program);
        void SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component, ECS::STransformComponent& _Player);
        void SetViewMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component, Core::CEvent& _Event, ECS::STransformComponent& _Player, ECS::CViewComponent& _view_Component);
    };
}
    
#endif
