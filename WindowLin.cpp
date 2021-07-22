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
#include "Event.h"

namespace GLVM::Core
{    
    CWindowLin::CWindowLin()
    {
  
//          Window RootWin;
//        XVisualInfo *pVisual;
//        Colormap ColorMap;

        //  const int aAttrib[13];
//        XSetWindowAttributes Swa;

        const int aAttrib[] =
        {
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_DOUBLEBUFFER, true,
            GLX_RED_SIZE, 1,
            GLX_GREEN_SIZE, 1,
            GLX_BLUE_SIZE, 1,
            None
        };

        
        pDisp_ = XOpenDisplay(NULL);
        
        pFbc_ = glXChooseFBConfig(pDisp_,
                                         DefaultScreen(pDisp_),
                                         aAttrib, &iNum_Fbc_);

        if (!pFbc_)
        {
            printf("glXChooseFBConfig() failed\n");
        exit(1);
        }

        pVisual_ = glXGetVisualFromFBConfig(pDisp_, pFbc_[0]);

        if(pVisual_ == NULL)
        {
            printf("\n\tno appropriate visual found\n\n");
            exit(0);
        } 
        else
        {
            printf("\n\tvisual %p selected\n", (void *)pVisual_->visualid); /* %p creates hexadecimal output like in glxinfo */
        }

        Root_Window_ = DefaultRootWindow(pDisp_);
        Color_Map_ = XCreateColormap(pDisp_, Root_Window_, pVisual_->visual, AllocNone);

        Set_Window_Attributes_.colormap = Color_Map_;
        Set_Window_Attributes_.event_mask = ExposureMask | KeyPressMask;

        Win_ = XCreateWindow(pDisp_, Root_Window_, 0, 0, 600, 600, 0, pVisual_->depth, InputOutput,
                            pVisual_->visual, CWColormap | CWEventMask, &Set_Window_Attributes_);    

    
        pGLXCreateContextAttribsARB_ =
            (GLXContext (*)(Display*, GLXFBConfig, GLXContext, Bool, const int*))
        glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

        if (!pGLXCreateContextAttribsARB_)
        {
            printf("glXCreateContextAttribsARB() not found\n");
            exit(1);
        }

        /* Set desired minimum OpenGL version */
        int context_attribs[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
                GLX_CONTEXT_MINOR_VERSION_ARB, 2,
                None
            };
        /* Create modern OpenGL context */
        Context_ = pGLXCreateContextAttribsARB_(pDisp_, pFbc_[0], NULL, true,
                                                    context_attribs);
        if (!Context_)
        {
            printf("Failed to create OpenGL context. Exiting.\n");
            exit(1);
        }

        /* Show_the_window
           --------------- */
        XMapWindow(pDisp_, Win_);
        glXMakeCurrent(pDisp_, Win_, Context_);

        int major = 0, minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        printf("OpenGL context created.\nVersion %d.%d\nVendor %s\nRenderer %s\n",
               major, minor,
               glGetString(GL_VENDOR),
               glGetString(GL_RENDERER));

//        glEnable(GL_DEPTH_TEST);

        XGetWindowAttributes(pDisp_, Win_, &GWindow_Attributes_); // !!!
//        glViewport(0, 0, Gwa.width, Gwa.height); // !!!
    }

    CWindowLin::~CWindowLin()
    {
/*        glXDestroyContext(pDisp_, GLContext_);
        XDestroyWindow(pDisp_, Win_);
        XFreeColormap(pDisp_, Color_Map_);
        XFree(pVisual_);
        XFree(pFbc_);
        XCloseDisplay(pDisp_);*/
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

    XWindowAttributes& CWindowLin::GetGwa()
    {
        return GWindow_Attributes_;
    }

    void CWindowLin::SwapBuffers()
    {
        glXSwapBuffers(pDisp_, Win_);
    }

    GLXContext& CWindowLin::GetModernContext()
    {
        return Context_;
    }

    void CWindowLin::ClearDisplay()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    Colormap& CWindowLin::GetColorMap()
    {
        return Color_Map_;
    }

    XVisualInfo* CWindowLin::GetVisual()
    {
        return pVisual_;
    }

    GLXFBConfig* CWindowLin::GetConfig()
    {
        return pFbc_;
    }

    void CWindowLin::HandleEvent(CEvent& Event)
    {
        
    }
}


