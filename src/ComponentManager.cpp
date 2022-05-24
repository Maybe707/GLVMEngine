#include "ComponentManager.hpp"

namespace GLVM::ECS
{
    CComponentManager* CComponentManager::pInstance_ = nullptr;
    std::mutex CComponentManager::Mutex_;

    CComponentManager::CComponentManager() {}
    
    CComponentManager::~CComponentManager()
    {
        for(int i = 0, iSize_Main = tWorld_Components_Container_.GetSize(); i < iSize_Main; ++i)
        {
            delete tWorld_Components_Container_[i];
            tWorld_Components_Container_[i] = nullptr;
        }
        for(int j = 0, iSize_Ordered = tWorld_IDs_Container.GetSize(); j < iSize_Ordered; ++j)
        {
            delete tWorld_IDs_Container[j];
            tWorld_IDs_Container[j] = nullptr;
        }
    }

    unsigned int CComponentManager::GetContainerID()
    {
        return s_iComponents_Container_ID;
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
