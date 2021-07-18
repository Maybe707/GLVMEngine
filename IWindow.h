#ifndef IWINDOW
#define IWINDOW

#include <X11/Xlib.h>
#include <GL/glx.h>

namespace GLVM::Core
{    

    class IWindow
    {
    public:
        virtual ~IWindow() {}

        virtual Display* GetDisplay() = 0;
        virtual Window& GetWindow() = 0;
        virtual GLXContext& GetContext() = 0;
    };

}
    
#endif
