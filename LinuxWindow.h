#ifndef LINUX_WINDOW
#define LINUX_WINDOW

#include <X11/X.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include "IWindow.h"

namespace GLVM::Core
{    

    class CLinuxWindow : public IWindow
    {
        Display* pDisp;
        Window Win;
        GLXContext GLContext;

    public:
        CLinuxWindow();

        Display* GetDisplay() override;
        Window& GetWindow() override;
        GLXContext& GetContext() override;
    };
}
    
#endif


