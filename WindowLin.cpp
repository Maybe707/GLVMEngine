#include "WindowLin.h"

#include "GLPointer.h"
#include "IWindow.h"
//#include "GLPointer.h"
#include "Event.h"

namespace GLVM::Core
{    
    CWindowLin::CWindowLin()
    {
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
            
            ///< creates hexadecimal output like in glxinfo
            
            printf("\n\tvisual %p selected\n", (void *)pVisual_->visualid); 
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

        ///< Set desired minimum OpenGL version
        
        int aContext_Attribs[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
                GLX_CONTEXT_MINOR_VERSION_ARB, 2,
                None
            };
        
        ///< Create modern OpenGL context
        
        Context_ = pGLXCreateContextAttribsARB_(pDisp_, pFbc_[0], NULL, true,
                                                    aContext_Attribs);
        if (!Context_)
        {
            printf("Failed to create OpenGL context. Exiting.\n");
            exit(1);
        }

        ///< Show_the_window
        
        XMapWindow(pDisp_, Win_);
        glXMakeCurrent(pDisp_, Win_, Context_);

        int iMajor = 0, iMinor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &iMajor);
        glGetIntegerv(GL_MINOR_VERSION, &iMinor);
        printf("OpenGL context created.\nVersion %d.%d\nVendor %s\nRenderer %s\n",
               iMajor, iMinor,
               glGetString(GL_VENDOR),
               glGetString(GL_RENDERER));

        ///< glEnable(GL_DEPTH_TEST);

        XGetWindowAttributes(pDisp_, Win_, &GWindow_Attributes_);
        Initializer();
    }

    CWindowLin::~CWindowLin()
    {
/*!        glXDestroyContext(pDisp_, GLContext_);
        XDestroyWindow(pDisp_, Win_);
        XFreeColormap(pDisp_, Color_Map_);
        XFree(pVisual_);
        XFree(pFbc_);
        XCloseDisplay(pDisp_);*/
    }

    void CWindowLin::SwapBuffers()
    {
        glXSwapBuffers(pDisp_, Win_);
    }

    void CWindowLin::ClearDisplay()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void CWindowLin::HandleEvent(CEvent& _Event)
    {
        XEvent Xevent;
        
        if(XPending(pDisp_))
        {
            XNextEvent(pDisp_, &Xevent);

            if(Xevent.type == KeyPress)
            {
                _Event.SetEvent(EEvents::eEXIT);
                _Event.SetReturnFlag(true);
            }
        }
    }

    void CWindowLin::Close()
    {
        glXMakeCurrent(pDisp_, None, NULL);
        glXDestroyContext(pDisp_, Context_);
        XDestroyWindow(pDisp_, Win_);
        XFreeColormap(pDisp_, Color_Map_);
        XFree(pVisual_);
        XFree(pFbc_);
        XCloseDisplay(pDisp_);
    }
}


