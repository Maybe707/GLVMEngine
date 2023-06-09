#include "Systems/RenderSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/TransformComponent.hpp"
#include "TextureManager.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"

namespace GLVM::ecs
{
    CRenderSystem::CRenderSystem()
	{
        GLVM::ecs::TextureManager* textureManager = GLVM::ecs::TextureManager::GetInstance();
        GLVM::ecs::TextureManager* hudTextureManager = GLVM::ecs::TextureManager::GetHUDInstance();
        GLVM::core::MeshManager*   meshManager = GLVM::core::MeshManager::GetInstance();

#ifdef VULKAN_API        
        renderer_instance_ = new core::CVulkanRenderer(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
#endif
		
#ifdef OPENGL_API
        renderer_instance_ = new core::COpenglRenderer();
#endif

        renderer_instance_->SetTextureData(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
        renderer_instance_->SetMeshData(meshManager->pathsArray_, meshManager->pathsGLTF_);
        renderer_instance_->run();
	}

    void CRenderSystem::SetTextureData() {
        GLVM::ecs::TextureManager* textureManager = GLVM::ecs::TextureManager::GetInstance();
        GLVM::ecs::TextureManager* hudTextureManager = GLVM::ecs::TextureManager::GetHUDInstance();

        renderer_instance_->SetTextureData(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
    }
    
	CRenderSystem::~CRenderSystem() {}
    void CRenderSystem::Update() {
//        SetTextureData();
        renderer_instance_->draw();
    }
    core::IRenderer* CRenderSystem::GetRenderSystemInstance() { return renderer_instance_; }    
	void CRenderSystem::SetModelMatrix()
	{
	}

    void CRenderSystem::SetViewMatrix(mat4 _viewMatrix) {
        renderer_instance_->SetViewMatrix(_viewMatrix);
    }
    
    void CRenderSystem::SetProjectionMatrix(mat4 _projectionMatrix) {
        renderer_instance_->SetProjectionMatrix(_projectionMatrix);
    }
}
