#ifndef RENDER_SYSTEM
#define RENDER_SYSTEM

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
#include "GraphicAPI/Opengl.hpp"
//#include "SpritesData.hpp"

//#define OPENGL

// #ifdef VULKAN
// #include "GraphicAPI/Vulkan.hpp"
// #endif

// #ifdef OPENGL
// #include "GraphicAPI/Opengl.hpp"
// #endif

#include "GraphicAPI/Opengl.hpp"

/*! \class Renderer.
    \brief Render all game objects.

    Take a game object to render in DrawSprite method.
*/

namespace GLVM::ECS
{    
    class CRenderSystem : public ISystem
    {
	public:
#ifdef VULKAN
        Core::CVulkanRenderer* vk_;

        CRenderSystem(std::vector<Core::Texture> _texture_data);
        void SetTexture(std::vector<Core::Texture> _texture_data);
        Core::CVulkanRenderer* GetVkRenderSystem();
#endif

#ifdef OPENGL
        Core::COpenglRenderer* gl_;

        CRenderSystem();
        Core::COpenglRenderer* GetGlRendersystem();
#endif

		~CRenderSystem();

		void Update() override;
        void SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component);
    };
}
    
#endif
