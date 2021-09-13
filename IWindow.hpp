#ifndef IWINDOW
#define IWINDOW

//#include <X11/Xlib.h>
//#include <GL/glx.h>
#include "Event.hpp"

namespace GLVM::Core
{    

    class IWindow
    {
    public:
        virtual ~IWindow() {}

        virtual void SwapBuffers() = 0;
        virtual void ClearDisplay() = 0;
        virtual void HandleEvent(CEvent& _Event) = 0;
        virtual void Close() = 0;
    };

}
    
#endif
