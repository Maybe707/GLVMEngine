#ifndef IWINDOW
#define IWINDOW

#include "Event.hpp"

namespace GLVM::Core
{    

    class IWindow
    {
    public:
        virtual ~IWindow() {}

        virtual void SwapBuffers() = 0;
        virtual void ClearDisplay() = 0;
        virtual bool HandleEvent(CEvent& _Event) = 0;
        virtual void Close() = 0;
    };

}
    
#endif
