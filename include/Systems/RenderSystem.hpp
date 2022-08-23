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
//#include "SpritesData.hpp"

//#define OPENGL

// #ifdef VULKAN
// #include "GraphicAPI/Vulkan.hpp"
// #endif

// #ifdef OPENGL
// #include "GraphicAPI/Opengl.hpp"
// #endif

//#include "GraphicAPI/Opengl.hpp"
#include "GraphicAPI/Vulkan.hpp"

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
#ifdef VULKAN
    private:
//        Core::CVulkanRenderer* vk_;

    public:
        void SetTexture(std::vector<ECS::CTextureComponent> _texture_data);
#endif

#ifdef OPENGL
    private:
//        Core::COpenglRenderer* gl_;
#endif

    public:
		~CRenderSystem();

		void Update() override;
        CRenderSystem();
        Core::IRenderer* GetRenderSystemInstance();
        void SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component);
    };
}
    
#endif
