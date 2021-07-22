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

        Display* GetDisplay() override;
        Window& GetWindow() override;
        GLXContext& GetContext() override;
        XWindowAttributes& GetGwa() override;
        virtual GLXContext& GetModernContext() override;
        void SwapBuffers() override;
        void ClearDisplay() override;
        Colormap& GetColorMap() override;
        XVisualInfo* GetVisual() override;
        GLXFBConfig* GetConfig() override;
        void HandleEvent(CEvent& Event) override;
    };
}
    
#endif


