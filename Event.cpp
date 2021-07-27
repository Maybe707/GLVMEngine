#include "Event.h"

namespace GLVM::Core
{
    CEvent::CEvent()
    {
        bReturn_Flag_ = false;
    }
    
    EEvents& CEvent::GetEvent()
    {
        return eEvent_;
    }

    bool& CEvent::GetReturnFlag()
    {
        return bReturn_Flag_;
    }

    void CEvent::SetEvent(EEvents _eEvent)
    {
        eEvent_ = _eEvent;
    }

    void CEvent::SetReturnFlag(bool _bReturn_Flag)
    {
        bReturn_Flag_ = _bReturn_Flag;
    }
}
