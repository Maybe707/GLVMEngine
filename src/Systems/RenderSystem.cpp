#include "Systems/RenderSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/TransformComponent.hpp"
#include "VectorContainer.hpp"

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

        Core::TCVectorContainer<unsigned int>* pEntity_Container_refTransform =
            ECS::GetInnerIDsContainer<ECS::STransformComponent>(*pComponent_Manager);
        unsigned int uiVector_Transform_Size = pEntity_Container_refTransform->GetSize();
        
        std::vector<ECS::CTextureComponent> temp_texture_vector;
        for(int i = 0, iSize = uiVector_Texture_Size; i < iSize; ++i) {
            unsigned int uiEntity_refTexture = (*pEntity_Container_refTexture)[i];
            temp_texture_vector.push_back(pComponent_Manager->GetComponent<ECS::CTextureComponent>(uiEntity_refTexture));
        }

        std::vector<ECS::STransformComponent> temp_transform_vector;
        for(int i = 0, iSize = uiVector_Transform_Size; i < iSize; ++i) {
            unsigned int uiEntity_refTransform = (*pEntity_Container_refTransform)[i];
            temp_transform_vector.push_back(pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refTransform));
        }
        
        renderer_instance_ = new Core::CVulkanRenderer(temp_texture_vector);
        renderer_instance_->SetTextureData(temp_texture_vector);
        renderer_instance_->SetTransformData(temp_transform_vector);
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

    void CRenderSystem::SetTransformData(std::vector<ECS::STransformComponent> _transform_data) {
        renderer_instance_->SetTransformData(_transform_data); 
    }
#endif
    
	CRenderSystem::~CRenderSystem() {}
    void CRenderSystem::Update() { renderer_instance_->draw(); }
    Core::IRenderer* CRenderSystem::GetRenderSystemInstance() { return renderer_instance_; }    
	void CRenderSystem::SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component)
	{
	}
}
