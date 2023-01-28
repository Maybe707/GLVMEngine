#include "TextureManager.hpp"
#include "Components/MaterialComponent.hpp"
#include "Texture.hpp"
#include <iostream>

namespace GLVM::ecs
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
		for ( unsigned int i = 0; i < textureVector_.size(); ++i ) {
			LoadTextureData(textureVector_[i]);
		}
    }

 	void CTextureManager::LoadTextureData(GLVM::ecs::CTexture& _Texture)
	{
		///< Loading and creating texture.
		glGenTextures(NUMBER_OF_CREATING_TEXTURE_OBJECT_1, &_Texture.iTexture_);
		glBindTexture(GL_TEXTURE_2D, _Texture.iTexture_);

		///< Loading image, creating texture and generation mipmap-levels
		glTexImage2D(GL_TEXTURE_2D, MIPMAP_LEVEL, GL_RGBA, _Texture.iWidth_, _Texture.iHeight_, SOME_OLD_STUFF, GL_RGBA, GL_UNSIGNED_BYTE, _Texture.u_iData_);
		pGLGenerate_Mipmap(GL_TEXTURE_2D);

		///< Setting texture applying parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
    std::vector<CTexture>& CTextureManager::GetTextureVector() { return textureVector_; }
    void CTextureManager::UnbindTexture(components::material _textureComponent, Entity _entity) {
        std::vector<Entity>& textureVector = textureVector_[_textureComponent.diffuseTextureID_].entitiesOwnsThisTypeOfTexture_;
        for (unsigned int i = 0; i < textureVector.size(); ++i) {
            if (textureVector[i] == _entity)
                textureVector.erase(textureVector.begin() + i);
            else
                continue;
        }
    }
}
