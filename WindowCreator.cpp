#include "WindowCreator.h"
#include "IWindow.h"
#include "LinuxWindow.h"

namespace GLVM::Core
{    
        IWindow* CWindowCreator::Create()
        {
#ifdef __linux__ 
            return new CLinuxWindow;
#endif

#ifdef _WIN32
            return new CWinWindow;
#endif
        }
}
