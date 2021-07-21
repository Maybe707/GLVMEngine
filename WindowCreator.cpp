#include "WindowCreator.h"
#include "IWindow.h"
#include "WindowLin.h"

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
