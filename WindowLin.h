#ifndef WINDOW_LIN
#define WINDOW_LIN

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include "IWindow.h"

namespace GLVM::Core
{    

    class CWindowLin : public IWindow
    {
        Display* pDisp_;
        Window Win_;
        GLXContext GLContext_;
        int iNum_Fbc_ = 0;
        GLXContext (*pGLXCreateContextAttribsARB_) (Display*, GLXFBConfig,
                                                  GLXContext, Bool, const int*) = 0;
        GLXContext Context_;
        XWindowAttributes GWindow_Attributes_;
        Window Root_Window_;
        XSetWindowAttributes Set_Window_Attributes_;
        Colormap Color_Map_;
        XVisualInfo* pVisual_;
        GLXFBConfig* pFbc_;

    public:
        
        CWindowLin();
        ~CWindowLin();


        void SwapBuffers() override;
        void ClearDisplay() override;
        void HandleEvent(CEvent& _Event) override;
        void Close() override;
    };
}
    
#endif


