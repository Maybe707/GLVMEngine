#ifndef IRENDERER
#define IRENDERER

#include <vector>
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Texture.hpp"

namespace GLVM::Core
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() {}

        virtual void draw() = 0;
        virtual void SetTransformData(std::vector<ECS::STransformComponent> _transform_data) = 0;
        virtual void SetTextureData(std::vector<ECS::CTexture> _texture_data) = 0;
        virtual void SetViewMatrix(mat4 _viewMatrix) = 0;
        virtual void SetProjectionMatrix(mat4 _projectionMatrix) = 0;
        virtual void run() = 0;
    };
}

#endif
