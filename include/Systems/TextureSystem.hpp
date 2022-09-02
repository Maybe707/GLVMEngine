#ifndef TEXTURE_SYSTEM
#define TEXTURE_SYSTEM

#include <vector>
#include "Texture.hpp"
#include <mutex>
#include "Components/TextureComponent.hpp"

typedef unsigned int Entity_ID;
typedef unsigned int Texture_ID;

namespace GLVM::ECS
{
    class CTextureSystem
    {
        static CTextureSystem* pInstance_;
        static std::mutex  Mutex_;
        
        std::vector<CTexture> textureVector_;
    public:
        CTextureSystem();

        void SetTextureVector(std::vector<CTexture> _textureVector);
        static CTextureSystem* GetInstance(); ///< It possibly to get only one instance of this class whith this method.
        void BindTexture(Entity_ID _entityID, Texture_ID _textureID);
        std::vector<CTexture>& GetTextureVector();
        void UnbindTexture(CTextureComponent _textureComponent, Entity _entity);
    };
}

#endif
