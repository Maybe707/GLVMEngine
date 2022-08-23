#ifndef IRENDERER
#define IRENDERER

#include <vector>
#include "Components/TextureComponent.hpp"

namespace GLVM::Core
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() {}

        virtual void draw() = 0;
        virtual void SetTextureData(std::vector<ECS::CTextureComponent> _texture_data) = 0;
        virtual void run() = 0;
    };
}

#endif
