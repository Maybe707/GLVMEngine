#include "Event.h"

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
}
