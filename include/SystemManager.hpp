#ifndef SYSTEM_MANAGER
#define SYSTEM_MANAGER

#include "ISystem.hpp"
#include "VectorContainer.hpp"
#include <mutex>

namespace GLVM::ecs
{
	class CSystemManager : public ISystem
	{
        static CSystemManager* pInstance_;
        static std::mutex Mutex_;

        CSystemManager();
        ~CSystemManager();
        
	public:
        CSystemManager(CSystemManager& _system_Manager)       = delete;    ///< Dont need to make cope because of singleton property.
        void operator=(const CSystemManager& _system_Manager) = delete;    ///< Dont need assignment operator because of singleton property.
        static CSystemManager* GetInstance();                     ///< It possibly to get only one instance of this class whith this method.
        
		inline static unsigned int s_iSystem_ID = 0;
		core::vector<ISystem*> tSystemContainer;

		void ActivateSystem(ISystem* _System);

		void Update() override;
	};
}

#endif
