#include "MeshManager.hpp"
#include "Components/VertexComponent.hpp"

namespace GLVM::core
{    
    CMeshManager* CMeshManager::pInstance_ = nullptr;
    std::mutex CMeshManager::Mutex_;

    CMeshManager::CMeshManager() {}
    CMeshManager::~CMeshManager() {}

    void CMeshManager::SetMesh(const char* _pathToMesh) {
        pathsArray_.push_back(_pathToMesh);
    }
    
    CMeshManager* CMeshManager::GetInstance()
    {
        
        std::lock_guard<std::mutex> lock(Mutex_);
        if(pInstance_ == nullptr)
        {
            pInstance_ = new CMeshManager();
        }
        return pInstance_;
    }
}    
