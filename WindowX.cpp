#include "WindowX.hpp"

#include "GLPointer.h"
#include "Event.hpp"
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <bits/types/time_t.h>
#include <iostream>

namespace GLVM::Core
{    
    CWindowX::CWindowX()
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
        Set_Window_Attributes_.event_mask = KeyPressMask | KeyReleaseMask | PointerMotionMask;

        Win_ = XCreateWindow(pDisp_, Root_Window_, 0, 0, 1920, 1080, 0, pVisual_->depth, InputOutput,
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
		Drawable = glXGetCurrentDrawable();
		Initializer();
		const int kInterval = 1;

		if (Drawable)
		{
			pGLXSwap_Interval_EXT(pDisp_, Drawable, kInterval);
		}
    }

    CWindowX::~CWindowX()
    {
/*!        glXDestroyContext(pDisp_, GLContext_);
        XDestroyWindow(pDisp_, Win_);
        XFreeColormap(pDisp_, Color_Map_);
        XFree(pVisual_);
        XFree(pFbc_);
        XCloseDisplay(pDisp_);*/
    }

    void CWindowX::SwapBuffers()
    {
        glXSwapBuffers(pDisp_, Win_);
    }

    void CWindowX::ClearDisplay()
    {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool CWindowX::HandleEvent(CEvent& _Event)
    {
        XEvent uXEvent;

        while(XPending(pDisp_))
        {
            XNextEvent(pDisp_, &uXEvent);
			KeySym ulKey;
            
            XMotionEvent motion;
            int iNumber_Of_Screens;
            Window* pRoot_Windows;
            Window Root_Window_Returned;
            Window Local_Window_Returned;
            unsigned int u_iMask_Return;
            bool bPointer_Search_Flag;
            int iRoot_X, iRoot_Y;
            int iLocal_Window_X, iLocal_Window_Y;

			switch(uXEvent.type)
			{
            case MotionNotify:
                iNumber_Of_Screens = XScreenCount(pDisp_);
                pRoot_Windows = (unsigned long*)malloc(sizeof(Window) * iNumber_Of_Screens);
                for (int i = 0; i < iNumber_Of_Screens; ++i)
                {
                    pRoot_Windows[i] = XRootWindow(pDisp_, i);
                }
                for(int j = 0; j < iNumber_Of_Screens; ++j)
                {
                bPointer_Search_Flag = XQueryPointer(pDisp_, pRoot_Windows[j], &Root_Window_Returned, &Local_Window_Returned, &iRoot_X,
                                                     &iRoot_Y, &iLocal_Window_X, &iLocal_Window_Y, &u_iMask_Return);
                }
                XQueryPointer(pDisp_, Local_Window_Returned, &Root_Window_Returned, &Local_Window_Returned, &iRoot_X, &iRoot_Y,
                              &iLocal_Window_X, &iLocal_Window_Y, &u_iMask_Return);
                std::cout << "Root window: " << Root_Window_Returned << std::endl;
                std::cout << "Local window: " << Local_Window_Returned << std::endl;
                std::cout << "Root window pointer posotion:  " << iRoot_X << " " << iRoot_Y << std::endl;
                std::cout << "Local window pointer posotion: : " << iLocal_Window_X << " " << iLocal_Window_Y << std::endl;
                motion = uXEvent.xmotion;
                _Event.SetEvent(EEvents::eMOUSE_POINTER_POSITION);
                _Event.mouse_Pointer_Position_.u_iX = iLocal_Window_X;
                _Event.mouse_Pointer_Position_.u_iY = iLocal_Window_Y;
                break;
			case KeyPress:
				ulKey = XLookupKeysym(&uXEvent.xkey, 0);
				switch(ulKey)
				{
				case XK_Escape:
					_Event.SetEvent(EEvents::eGAME_LOOP_KILL);
					break;
				case XK_a:
					_Event.SetEvent(EEvents::eMOVE_LEFT);
					break;
				case XK_d:
					_Event.SetEvent(EEvents::eMOVE_RIGHT);
					break;
				case XK_s:
					_Event.SetEvent(EEvents::eMOVE_DOWN);
					break;
				case XK_w:
					_Event.SetEvent(EEvents::eMOVE_UP);
					break;
				}
				break;

			case KeyRelease:
				if(XEventsQueued(pDisp_, QueuedAfterReading))
				{
					XEvent uXNext_Event;
					XPeekEvent(pDisp_, &uXNext_Event);
    
					if (uXNext_Event.type == KeyPress && uXNext_Event.xkey.time == uXEvent.xkey.time &&
						uXNext_Event.xkey.keycode == uXEvent.xkey.keycode)
					{
						///< Key wasn’t actually released
                        XNextEvent(pDisp_, &uXNext_Event);
						continue;
					}
				}
		    	ulKey = XLookupKeysym(&uXEvent.xkey, 0);
                switch(ulKey)
                {
                case XK_a:
                    _Event.SetEvent(GLVM::Core::eKEYRELEASE_A);
                    break;
                case XK_d:
                    _Event.SetEvent(GLVM::Core::eKEYRELEASE_D);
                    break;
                case XK_s:
                    _Event.SetEvent(GLVM::Core::eKEYRELEASE_S);
                    break;
                case XK_w:
                    _Event.SetEvent(GLVM::Core::eKEYRELEASE_W);
                    break;
                }
				break;
			}
			return true;
        }
		return false;
    }

    void CWindowX::Close()
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


