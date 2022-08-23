#include "Systems/RenderSystem.hpp"

namespace GLVM::ECS
{
    CRenderSystem::CRenderSystem()
	{
#ifdef OPENGL_API
        renderer_instance_ = new Core::COpenglRenderer();
#endif
        
#ifdef VULKAN_API
        ECS::CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refTexture =
            ECS::GetInnerIDsContainer<ECS::CTextureComponent>(*pComponent_Manager);
        unsigned int uiVector_Texture_Size = pEntity_Container_refTexture->GetSize();

        std::cout << "Size: " << uiVector_Texture_Size << std::endl;
        
        std::vector<ECS::CTextureComponent> temp_vector;
        for(int i = 0, iSize = uiVector_Texture_Size; i < iSize; ++i)
        {
            unsigned int uiEntity_refTexture= (*pEntity_Container_refTexture)[i];
            temp_vector.push_back(pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture));
        }
        
        renderer_instance_ = new Core::CVulkanRenderer(temp_vector);
        renderer_instance_->SetTextureData(temp_vector);
        renderer_instance_->run();
#endif
	}

#ifdef VULKAN_API
    void CRenderSystem::SetTextureData(std::vector<ECS::CTextureComponent> _texture_data) {
        delete renderer_instance_;
        renderer_instance_ = nullptr;
        renderer_instance_ = new Core::CVulkanRenderer(_texture_data);
        renderer_instance_->SetTextureData(_texture_data);
        renderer_instance_->run();
    }
#endif
    
	CRenderSystem::~CRenderSystem() {}
    void CRenderSystem::Update() { renderer_instance_->draw(); }
    Core::IRenderer* CRenderSystem::GetRenderSystemInstance() { return renderer_instance_; }    
	void CRenderSystem::SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component)
	{
	}
}
