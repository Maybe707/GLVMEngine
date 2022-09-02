#ifndef TEXTURE_MANAGER
#define TEXTURE_MANAGER

#include <vector>
#include "Texture.hpp"
#include <mutex>
#include "Components/TextureComponent.hpp"

typedef unsigned int Entity_ID;
typedef unsigned int Texture_ID;

namespace GLVM::ECS
{
    class CTextureManager
    {
        static CTextureManager* pInstance_;
        static std::mutex  Mutex_;
        
        std::vector<CTexture> textureVector_;
    public:
        CTextureManager();

        void SetTextureVector(std::vector<CTexture> _textureVector);
        static CTextureManager* GetInstance(); ///< It possibly to get only one instance of this class whith this method.
        void BindTexture(Entity_ID _entityID, Texture_ID _textureID);
        std::vector<CTexture>& GetTextureVector();
        void UnbindTexture(CTextureComponent _textureComponent, Entity _entity);
    };
}

#endif
