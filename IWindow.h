#ifndef IWINDOW
#define IWINDOW

#include <X11/Xlib.h>
#include <GL/glx.h>
#include "Event.h"

namespace GLVM::Core
{    

    class IWindow
    {
    public:
        virtual ~IWindow() {}

        virtual Display* GetDisplay() = 0;
        virtual Window& GetWindow() = 0;
        virtual GLXContext& GetContext() = 0;
        virtual XWindowAttributes& GetGwa() = 0;
        virtual GLXContext& GetModernContext() = 0;
        virtual void SwapBuffers() = 0;
        virtual void ClearDisplay() = 0;
        virtual Colormap& GetColorMap() = 0;
        virtual XVisualInfo* GetVisual() = 0;
        virtual GLXFBConfig* GetConfig() = 0;
        virtual void HandleEvent(CEvent& Event) = 0;
    };

}
    
#endif
