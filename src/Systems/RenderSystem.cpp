#include "Systems/RenderSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/TransformComponent.hpp"
#include "TextureManager.hpp"
#include "VectorContainer.hpp"
#include "VertexMath.hpp"

namespace GLVM::ECS
{
    CRenderSystem::CRenderSystem()
	{
#ifdef OPENGL_API
        renderer_instance_ = new Core::COpenglRenderer();
#endif
        
#ifdef VULKAN_API
        GLVM::ECS::CTextureManager* textureManager = GLVM::ECS::CTextureManager::GetInstance();
        GLVM::ECS::CTextureManager* hudTextureManager = GLVM::ECS::CTextureManager::GetHUDInstance();
        
        renderer_instance_ = new Core::CVulkanRenderer(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
        renderer_instance_->SetTextureData(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
        renderer_instance_->run();
#endif
	}

#ifdef VULKAN_API
    void CRenderSystem::SetTextureData() {
        GLVM::ECS::CTextureManager* textureManager = GLVM::ECS::CTextureManager::GetInstance();
        GLVM::ECS::CTextureManager* hudTextureManager = GLVM::ECS::CTextureManager::GetHUDInstance();
        
        renderer_instance_->SetTextureData(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
    }
#endif
    
	CRenderSystem::~CRenderSystem() {}
    void CRenderSystem::Update() {
#ifdef VULKAN_API
        SetTextureData();
#endif
        renderer_instance_->draw();
    }
    Core::IRenderer* CRenderSystem::GetRenderSystemInstance() { return renderer_instance_; }    
	void CRenderSystem::SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component)
	{
	}

    void CRenderSystem::SetViewMatrix(mat4 _viewMatrix) {
        renderer_instance_->SetViewMatrix(_viewMatrix);
    }
    
    void CRenderSystem::SetProjectionMatrix(mat4 _projectionMatrix) {
        renderer_instance_->SetProjectionMatrix(_projectionMatrix);
    }
}
