#ifndef RENDER_SYSTEM
#define RENDER_SYSTEM

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
#include "Components/TextureComponent.hpp"
#include "IRenderer.hpp"
#include <mutex>

#include "GraphicAPI/Opengl.hpp"
//#include "GraphicAPI/Vulkan.hpp"

/*! \class Renderer.
    \brief Render all game objects.

    Take a game object to render in DrawSprite method.
*/

namespace GLVM::ECS
{    
    class CRenderSystem : public ISystem
    {
    private:
        Core::IRenderer* renderer_instance_;
        
#ifdef VULKAN_API
    public:
        void SetTextureData(std::vector<ECS::CTextureComponent> _texture_data);
#endif

    public:
        CRenderSystem();
		~CRenderSystem();

		void Update() override;
        Core::IRenderer* GetRenderSystemInstance();
        void SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component);
        void SetViewMatrix(mat4 _viewMatrix);
        void SetProjectionMatrix(mat4 _projectionMatrix);
        void SetTransformData();
    };
}
    
#endif
