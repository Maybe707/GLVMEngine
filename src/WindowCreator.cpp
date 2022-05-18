#include "WindowCreator.hpp"
//#include "IWindow.hpp"

#ifdef __linux__
#include "UnixApi/WindowX.hpp"
#endif
#ifdef _WIN32
#include "WindowWin.hpp"
#endif

namespace GLVM::Core
{    
        IWindow* CWindowCreator::Create()
        {
#ifdef __linux__ 
            return new CWindowX;
#endif

#ifdef _WIN32
            return new CWindowWin;
#endif
        }
}
