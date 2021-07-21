#ifndef WINDOW_LIN
#define WINDOW_LIN

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

    class CWindowLin : public IWindow
    {
        Display* pDisp_;
        Window Win_;
        GLXContext GLContext_;

    public:
        CWindowLin();

        Display* GetDisplay() override;
        Window& GetWindow() override;
        GLXContext& GetContext() override;
        void SwapBuffers() override;
    };
}
    
#endif


