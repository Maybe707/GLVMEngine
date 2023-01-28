#include "Systems/RenderSystem.hpp"
#include "ComponentManager.hpp"
#include "Components/TransformComponent.hpp"
#include "MeshManager.hpp"
#include "TextureManager.hpp"
#include "VectorContainer.hpp"
#include "VertexMath.hpp"

namespace GLVM::ecs
{
    CRenderSystem::CRenderSystem()
	{
        GLVM::ecs::CTextureManager* textureManager = GLVM::ecs::CTextureManager::GetInstance();
        GLVM::ecs::CTextureManager* hudTextureManager = GLVM::ecs::CTextureManager::GetHUDInstance();
        GLVM::Core::CMeshManager*   meshManager = GLVM::Core::CMeshManager::GetInstance();

#ifdef VULKAN_API        
        renderer_instance_ = new Core::CVulkanRenderer(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
#endif
		
#ifdef OPENGL_API
        renderer_instance_ = new Core::COpenglRenderer();
#endif

        renderer_instance_->SetTextureData(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
        renderer_instance_->SetMeshData(meshManager->pathsArray_);
        renderer_instance_->run();
	}

    void CRenderSystem::SetTextureData() {
        GLVM::ecs::CTextureManager* textureManager = GLVM::ecs::CTextureManager::GetInstance();
        GLVM::ecs::CTextureManager* hudTextureManager = GLVM::ecs::CTextureManager::GetHUDInstance();
        
        renderer_instance_->SetTextureData(textureManager->GetTextureVector(), hudTextureManager->GetTextureVector());
    }
    
	CRenderSystem::~CRenderSystem() {}
    void CRenderSystem::Update() {
        SetTextureData();
        renderer_instance_->draw();
    }
    Core::IRenderer* CRenderSystem::GetRenderSystemInstance() { return renderer_instance_; }    
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
