#include "TextureManager.hpp"
#include "Components/TextureComponent.hpp"
#include "Texture.hpp"
#include <iostream>

namespace GLVM::ECS
{
    CTextureManager* CTextureManager::pInstance_ = nullptr;
    std::mutex CTextureManager::Mutex_;

    CTextureManager* CTextureManager::pHUDInstance_ = nullptr;
    std::mutex CTextureManager::HUDMutex_;
    
    CTextureManager::CTextureManager() {}

    void CTextureManager::BindTexture(Entity_ID _entityID, Texture_ID _textureID) {
        textureVector_[_textureID].entitiesOwnsThisTypeOfTexture_.push_back(_entityID);
    }

    CTextureManager* CTextureManager::GetInstance()
    {
        std::lock_guard<std::mutex> lock(Mutex_);
        if(pInstance_ == nullptr) {
            pInstance_ = new CTextureManager();
        }
        return pInstance_;
    }

    CTextureManager* CTextureManager::GetHUDInstance()
    {
        std::lock_guard<std::mutex> lock(HUDMutex_);
        if(pHUDInstance_ == nullptr) {
            pHUDInstance_ = new CTextureManager();
        }
        return pHUDInstance_;
    }
    
    void CTextureManager::SetTextureVector(std::vector<CTexture> _textureVector) {
        textureVector_ = _textureVector;
    }

    std::vector<CTexture>& CTextureManager::GetTextureVector() { return textureVector_; }
    void CTextureManager::UnbindTexture(CTextureComponent _textureComponent, Entity _entity) {
        std::vector<Entity>& textureVector = textureVector_[_textureComponent.id_].entitiesOwnsThisTypeOfTexture_;
        for (int i = 0; i < textureVector.size(); ++i) {
            if (textureVector[i] == _entity)
                textureVector.erase(textureVector.begin() + i);
            else
                continue;
        }
    }
}
