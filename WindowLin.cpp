#include "WindowLin.h"

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
#include "GLPointer.h"

namespace GLVM::Core
{    
    CWindowLin::CWindowLin()
    {
        XWindowAttributes Gwa;
        Window RootWin;
        XVisualInfo *pVisual;
        Colormap ColorMap;
        GLint aAttrib[5];
        XSetWindowAttributes Swa;
        aAttrib[0] = GLX_RGBA;
        aAttrib[1] = GLX_DEPTH_SIZE;
        aAttrib[2] = 24;
        aAttrib[3] = GLX_DOUBLEBUFFER;
        aAttrib[4] = None;
           
        pDisp_ = XOpenDisplay(NULL);

        if(pDisp_ == NULL)
        {
            printf("\n\tcannot connect to X server\n\n");
            exit(0);
        }

        RootWin = DefaultRootWindow(pDisp_);
    
        pVisual = glXChooseVisual(pDisp_, 0, aAttrib);

        if(pVisual == NULL) {
            printf("\n\tno appropriate visual found\n\n");
            exit(0);
        } 
        else {
            printf("\n\tvisual %p selected\n", (void *)pVisual->visualid); /* %p creates hexadecimal output like in glxinfo */
        }

        ColorMap = XCreateColormap(pDisp_, RootWin, pVisual->visual, AllocNone);

        Swa.colormap = ColorMap;
        Swa.event_mask = ExposureMask | KeyPressMask;

        Win_ = XCreateWindow(pDisp_, RootWin, 0, 0, 600, 600, 0, pVisual->depth, InputOutput,
                            pVisual->visual, CWColormap | CWEventMask, &Swa);

        XMapWindow(pDisp_, Win_);

        XStoreName(pDisp_, Win_, "VERY SIMPLE APPLICATION");

        GLContext_ = glXCreateContext(pDisp_, pVisual, NULL, GL_TRUE);
        glXMakeCurrent(pDisp_, Win_, GLContext_);
    
        glEnable(GL_DEPTH_TEST);

        XGetWindowAttributes(pDisp_, Win_, &Gwa); // !!!
        glViewport(0, 0, Gwa.width, Gwa.height); // !!!
    }

    Display* CWindowLin::GetDisplay()
    {
        return pDisp_;
    }

    Window& CWindowLin::GetWindow()
    {
        return Win_;
    }

    GLXContext& CWindowLin::GetContext()
    {
        return GLContext_;
    }

    void CWindowLin::SwapBuffers()
    {
        glXSwapBuffers(pDisp_, Win_);
    }
}


