#ifndef TEXTURE_MANAGER
#define TEXTURE_MANAGER

#include <vector>
#include "Texture.hpp"
#include <mutex>
#include "Components/MaterialComponent.hpp"
#include "GLPointer.h"
#include "Constants.hpp"

typedef unsigned int Entity_ID;
typedef unsigned int Texture_ID;

namespace GLVM::ecs
{
    class TextureManager
    {
        static TextureManager* pInstance_;
        static std::mutex  Mutex_;

        static TextureManager* pHUDInstance_;
        static std::mutex  HUDMutex_;
        
        std::vector<CTexture> textureVector_;
    public:
        TextureManager();
		
        void SetTextureVector(std::vector<CTexture> _textureVector);
        static TextureManager* GetInstance(); ///< It possibly to get only one instance of this class whith this method.
        static TextureManager* GetHUDInstance();
        void BindTexture(Entity_ID _entityID, Texture_ID _textureID);
		void LoadTextureData(GLVM::ecs::CTexture& _Texture);
        std::vector<CTexture>& GetTextureVector();
        void UnbindTexture(components::material _textureComponent, Entity _entity);
    };
}

#endif
