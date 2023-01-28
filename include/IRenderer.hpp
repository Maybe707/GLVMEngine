#ifndef IRENDERER
#define IRENDERER

#include <vector>
#include "Components/MaterialComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Texture.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace GLVM::core
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() {}

        virtual void draw() = 0;
        virtual void loadWavefrontObj() = 0;
        virtual void SetTextureData(std::vector<ecs::CTexture>& _texture_data, std::vector<ecs::CTexture>& _hud_texture_data) = 0;
        virtual void SetMeshData(std::vector<const char*> _pathsArray) = 0;
        virtual void SetViewMatrix(mat4 _viewMatrix) = 0;
        virtual void SetProjectionMatrix(mat4 _projectionMatrix) = 0;
        virtual void run() = 0;
    };
}

#endif
