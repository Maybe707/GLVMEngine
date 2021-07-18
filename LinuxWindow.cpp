#include "LinuxWindow.h"

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
    CLinuxWindow::CLinuxWindow()
    {
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
           
        pDisp = XOpenDisplay(NULL);

        if(pDisp == NULL)
        {
            printf("\n\tcannot connect to X server\n\n");
            exit(0);
        }

        RootWin = DefaultRootWindow(pDisp);
    
        pVisual = glXChooseVisual(pDisp, 0, aAttrib);

        if(pVisual == NULL) {
            printf("\n\tno appropriate visual found\n\n");
            exit(0);
        } 
        else {
            printf("\n\tvisual %p selected\n", (void *)pVisual->visualid); /* %p creates hexadecimal output like in glxinfo */
        }

        ColorMap = XCreateColormap(pDisp, RootWin, pVisual->visual, AllocNone);

        Swa.colormap = ColorMap;
        Swa.event_mask = ExposureMask | KeyPressMask;

        Win = XCreateWindow(pDisp, RootWin, 0, 0, 600, 600, 0, pVisual->depth, InputOutput,
                            pVisual->visual, CWColormap | CWEventMask, &Swa);

        XMapWindow(pDisp, Win);

        XStoreName(pDisp, Win, "VERY SIMPLE APPLICATION");

        GLContext = glXCreateContext(pDisp, pVisual, NULL, GL_TRUE);
        glXMakeCurrent(pDisp, Win, GLContext);
    
        glEnable(GL_DEPTH_TEST);
    }

    Display* CLinuxWindow::GetDisplay()
    {
        return pDisp;
    }

    Window& CLinuxWindow::GetWindow()
    {
        return Win;
    }

    GLXContext& CLinuxWindow::GetContext()
    {
        return GLContext;
    }
}


