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

namespace GLVM::ECS
{
    class CTextureManager
    {
        static CTextureManager* pInstance_;
        static std::mutex  Mutex_;

        static CTextureManager* pHUDInstance_;
        static std::mutex  HUDMutex_;
        
        std::vector<CTexture> textureVector_;
    public:
        CTextureManager();
		
        void SetTextureVector(std::vector<CTexture> _textureVector);
        static CTextureManager* GetInstance(); ///< It possibly to get only one instance of this class whith this method.
        static CTextureManager* GetHUDInstance();
        void BindTexture(Entity_ID _entityID, Texture_ID _textureID);
		void LoadTextureData(GLVM::ECS::CTexture& _Texture);
        std::vector<CTexture>& GetTextureVector();
        void UnbindTexture(ECS::material _textureComponent, Entity _entity);
    };
}

#endif
