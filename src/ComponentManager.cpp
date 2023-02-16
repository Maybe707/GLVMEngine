#include "ComponentManager.hpp"

namespace GLVM::ecs
{
    CComponentManager* CComponentManager::pInstance_ = nullptr;
    std::mutex CComponentManager::Mutex_;

    CComponentManager::CComponentManager() {}
    
    CComponentManager::~CComponentManager()
    {
        for(int i = 0, iSize_Main = worldComponentsContainer.GetSize(); i < iSize_Main; ++i)
        {
            delete worldComponentsContainer[i];
            worldComponentsContainer[i] = nullptr;
        }
        for(int j = 0, iSize_Ordered = worldSparseEntitiesMapToComponents.GetSize(); j < iSize_Ordered; ++j)
        {
            delete worldSparseEntitiesMapToComponents[j];
            worldSparseEntitiesMapToComponents[j] = nullptr;
        }
    }

    unsigned int CComponentManager::GetContainerID()
    {
        return componentsContainerID;
    }

    CComponentManager* CComponentManager::GetInstance()
    {
        std::lock_guard<std::mutex> lock(Mutex_);
        if(pInstance_ == nullptr)
        {
            pInstance_ = new CComponentManager();
        }
        return pInstance_;
    }
}
