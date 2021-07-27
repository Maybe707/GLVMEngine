#include "WindowCreator.h"
#include "IWindow.h"

#ifdef __linux__
#include "WindowLin.h"
#endif
#ifdef _WIN32
#include "WindowWin.h"
#endif

namespace GLVM::Core
{    
        IWindow* CWindowCreator::Create()
        {
#ifdef __linux__ 
            return new CWindowLin;
#endif

#ifdef _WIN32
            return new CWindowWin;
#endif
        }
}
