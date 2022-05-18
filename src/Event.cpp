#include "Event.hpp"
#include "Stack.hpp"

namespace GLVM::Core
{
    CEvent::CEvent()
    {
    }
    
    EEvents& CEvent::GetEvent()
    {
        return eEvent_;
    }

    void CEvent::SetEvent(EEvents _eEvent)
    {
        eEvent_ = _eEvent;
    }

	void CEvent::SetLastEvent(CStack _Stack)
	{
		switch(_Stack.Pop())
		{
		case GLVM::Core::eMOVE_RIGHT:
			SetEvent(GLVM::Core::EEvents::eMOVE_RIGHT);
			break;
		case GLVM::Core::eMOVE_LEFT:
			SetEvent(GLVM::Core::EEvents::eMOVE_LEFT);
			break;
		case GLVM::Core::eMOVE_BACKWARD:
			SetEvent(GLVM::Core::EEvents::eMOVE_BACKWARD);
			break;
		case GLVM::Core::eMOVE_FORWARD:
			SetEvent(GLVM::Core::EEvents::eMOVE_FORWARD);
			break;
		default:
			break;
		}
	}
}
