#include "MeshManager.hpp"
#include "Components/VertexComponent.hpp"

namespace GLVM::core
{    
    MeshManager* MeshManager::pInstance_ = nullptr;
    std::mutex MeshManager::Mutex_;

    MeshManager::MeshManager() {}
    MeshManager::~MeshManager() {}

    void MeshManager::SetMesh(const char* _pathToMesh) {
        pathsArray_.push_back(_pathToMesh);
    }
    
    MeshManager* MeshManager::GetInstance()
    {
        
        std::lock_guard<std::mutex> lock(Mutex_);
        if(pInstance_ == nullptr)
        {
            pInstance_ = new MeshManager();
        }
        return pInstance_;
    }
}    
