#include "WindowCreator.hpp"
#include "IWindow.hpp"

#ifdef __linux__
#include "WindowLin.hpp"
#endif
#ifdef _WIN32
#include "WindowWin.hpp"
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
