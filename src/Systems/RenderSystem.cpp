#include "Systems/RenderSystem.hpp"

namespace GLVM::ECS
{
#ifdef VULKAN
    CRenderSystem::CRenderSystem(std::vector<Core::Texture> _texture_data)
	{	
        vk_ = new Core::CVulkanRenderer(_texture_data);
        vk_->setTextureData(_texture_data);
        vk_->run();
	}

    void CRenderSystem::SetTexture(std::vector<Core::Texture> _texture_data) {
        delete vk_;
        vk_ = nullptr;
        vk_ = new Core::CVulkanRenderer(_texture_data);
        vk_->setTextureData(_texture_data);
        vk_->run();
    }
    
    Core::CVulkanRenderer* CRenderSystem::GetRenderSystemInstance() { return vk_; }
#endif

#ifdef OPENGL
    CRenderSystem::CRenderSystem()
	{	
        gl_ = new Core::COpenglRenderer();
	}

    Core::COpenglRenderer* CRenderSystem::GetRenderSystemInstance() { return gl_; }    
#endif
    
	CRenderSystem::~CRenderSystem() {}
    
	void CRenderSystem::Update()
	{
#ifdef VULKAN
        vk_->draw();
#endif

#ifdef OPENGL
        gl_->draw();
#endif
	}

    
    
	void CRenderSystem::SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component)
	{
        
	}
}
