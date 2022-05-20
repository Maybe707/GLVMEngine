#include "TimerCreator.hpp"

#ifdef __linux__
#include "UnixApi/ChronoX.hpp"
#endif
#ifdef _WIN32
#include "WinApi/ChronoWin.hpp"
#endif

namespace GLVM::Time
{    
        IChrono* CTimerCreator::Create()
        {
#ifdef __linux__ 
            return new CTimerX;
#endif

#ifdef _WIN32
            return new CTimerWin;
#endif
        }
}

