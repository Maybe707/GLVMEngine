#include "EntityManager.hpp"
#include "ComponentManager.hpp"

namespace GLVM::ECS
{
    CEntityManager* CEntityManager::pInstance_ = nullptr;
    std::mutex CEntityManager::Mutex_;
    
    CEntityManager::CEntityManager() {}
    CEntityManager::~CEntityManager() {}
    
    CEntityManager* CEntityManager::GetInstance()
    {
        std::lock_guard<std::mutex> lock(Mutex_);
        if(pInstance_ == nullptr)
        {
            pInstance_ = new CEntityManager();
        }
        return pInstance_;
    }
    
    [[nodiscard]] Entity_ID CEntityManager::CreateEntity()
    {
		Entity_ID _Entity_ID;
		
        if(tRemoved_Entity_Registry_.GetSize() > k_iNull)    ///< Check out wether or not free ID in removed entities registry.
        {
            _Entity_ID = tRemoved_Entity_Registry_.GetFirstItem();
            tActive_Entity_Registry_.Push(tRemoved_Entity_Registry_.GetFirstItem());
            tRemoved_Entity_Registry_.RemoveFirstItem();
        }
        else
        {
            tActive_Entity_Registry_.Push(u_iID);
            _Entity_ID = u_iID;
            ++u_iID;

        }
		return _Entity_ID;
    }

    /**************************************************************************************
     * Dont need to delete real component in this method. Because systems dont work with
     * component without indices for that component in ordered container.
     **************************************************************************************/
        
    void CEntityManager::RemoveEntity(Entity_ID& _Entity_ID, CComponentManager* _ComponentManager)
    {
        tRemoved_Entity_Registry_.Push(_Entity_ID);
        tActive_Entity_Registry_[_Entity_ID] = k_iUint_Max;  
        for(int i = 0, iSize = _ComponentManager->tWorld_IDs_Container.GetSize();
            i < iSize; ++i) {
            static_cast<Core::TCVectorContainer<unsigned int>*>(_ComponentManager->tWorld_IDs_Container[i])->RemoveItem(_Entity_ID);
        }
    }
}
