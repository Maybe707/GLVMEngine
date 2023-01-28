#ifndef RENDER_SYSTEM
#define RENDER_SYSTEM

#include "GLPointer.h"
#include <GL/gl.h>
#include "IRenderer.hpp"
#include "ISystem.hpp"
#include "VectorContainer.hpp"
#include "Components/VertexComponent.hpp"
#include "ComponentManager.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <GL/glext.h>
#include "Components/AnimationMoveComponent.hpp"
#include "VertexMath.hpp"
#include "Event.hpp"
#include "Components/ViewComponent.hpp"
#include "Constants.hpp"
#include "Components/MaterialComponent.hpp"
#include "IRenderer.hpp"
#include <mutex>
#include "TextureManager.hpp"

#include "GraphicAPI/Opengl.hpp"
//#include "GraphicAPI/Vulkan.hpp"

/*! \class Renderer.
    \brief Render all game objects.

    Take a game object to render in DrawSprite method.
*/

namespace GLVM::ecs
{    
  class CRenderSystem : public ISystem
  {
  private:
    Core::IRenderer* renderer_instance_;
        
  public:
    CRenderSystem();
    ~CRenderSystem();
    
    void Update() override;
    Core::IRenderer* GetRenderSystemInstance();
    void SetTextureData();
    void SetModelMatrix();
	  void SetViewMatrix(mat4 _viewMatrix);
	  void SetProjectionMatrix(mat4 _projectionMatrix);
  };
}
    
#endif
