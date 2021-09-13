#include "WindowWin.hpp"

namespace GLVM::Core
{    
    CWindowWin::CWindowWin()
    {
        HWND pClassic_Window_;
        HDC pClassic_DC_;
        HGLRC pClassic_Context_;

        ///< Create classic window
        pClassic_Window_ = CreateWindowA( "STATIC", "", WS_POPUP | WS_DISABLED, 0, 0, 1, 1, NULL, NULL, GetModuleHandle( NULL ), NULL );
        pClassic_DC_ = GetDC( pClassic_Window_ );

        ///< Set classic pixel format
        PIXELFORMATDESCRIPTOR classic_Format_Descriptor =
            {
                sizeof( classic_Format_Descriptor ), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0, PFD_MAIN_PLANE,
                0, 0, 0, 0
            };
        
        int iClassic_Pixel_Format = ChoosePixelFormat( pClassic_DC_, &classic_Format_Descriptor );
        SetPixelFormat( pClassic_DC_, iClassic_Pixel_Format, &classic_Format_Descriptor );

        ///< Create classic context
        pClassic_Context_ = wglCreateContext( pClassic_DC_ );
        wglMakeCurrent( pClassic_DC_, pClassic_Context_ );

        int iMajor, iMinor;
        glGetIntegerv( 4, &iMajor );
        glGetIntegerv( 2, &iMinor );
//		if ( iMajor < 3 || ( iMajor == 3 && iMinor < 2 ) ) throw VersionException();

        // Load OpenGL extensions
        //LoadExtensions();

        ///< Create final pixel format
        const int aPixel_Attribs[] =
            {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, 32,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                0
            };

        BOOL ( WINAPI * WGLChoosePixFormatARB ) ( HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats ) = (BOOL (WINAPI*)( HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats )) wglGetProcAddress((LPCSTR)"wglChoosePixelFormatARB");

        HGLRC ( WINAPI * WGLCreateContextAtribbARB ) ( HDC hDC, HGLRC hShareContext, const int* attribList ) = (HGLRC (WINAPI *) ( HDC hDC, HGLRC hShareContext, const int* attribList )) wglGetProcAddress((LPCSTR)"wglCreateContextAttribsARB");

        const char aClass_Name[] = "Sample Window Class";
            
        window_Class_ = { };

        window_Class_.style = WS_VISIBLE;
        window_Class_.lpfnWndProc = MainWndProc;
        window_Class_.cbClsExtra = 0;
        window_Class_.cbWndExtra = 0;
        window_Class_.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        window_Class_.hCursor = LoadCursor(NULL, IDC_SIZE);
        window_Class_.lpszClassName = (LPCSTR)aClass_Name;
        window_Class_.hInstance = GetModuleHandleA(NULL);
        window_Class_.lpszClassName = aClass_Name;

        RegisterClass(&window_Class_);
        
        pModern_Window_ = CreateWindowEx(0, aClass_Name, "Sample Window Class", WS_OVERLAPPEDWINDOW, 0, 0, 400, 400, NULL, NULL, GetModuleHandleA(NULL), NULL);
        ShowWindow(pModern_Window_, SW_SHOW);
        pModern_DC_ = GetDC( pModern_Window_ );
        
        int iModern_Pixel_Format;
        UINT pFormat_Count;
        WGLChoosePixFormatARB(pModern_DC_, aPixel_Attribs, NULL, 1, &iModern_Pixel_Format, &pFormat_Count );
//		if ( pFormat_Count == 0 ) throw PixelFormatException();
        SetPixelFormat( pModern_DC_, iModern_Pixel_Format, &classic_Format_Descriptor );
		
        ///< Create modern OpenGL 4.2 context		
        int aAttributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 2,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
		
        pModern_Context_ = WGLCreateContextAtribbARB( pModern_DC_, NULL, aAttributes );

        ///< Clean up
        wglMakeCurrent( pModern_DC_, pModern_Context_);

        Initializer();
    }

    void CWindowWin::SwapBuffers()
    {
        ::SwapBuffers(pModern_DC_);
    }

    void CWindowWin::ClearDisplay()
    {
        glClear( GL_COLOR_BUFFER_BIT );
    }

    void CWindowWin::HandleEvent(CEvent& _Event)
    {
        ///< Create message struct object.
        MSG msg;

        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            if(msg.message == WM_QUIT)
                _Event.SetEvent(EEvents::eEXIT);
        }
    }

    void CWindowWin::Close()
    {
        DestroyWindow(pModern_Window_);
    }
        
///< Callback method for events handling.
    LRESULT CALLBACK CWindowWin::MainWndProc(HWND _pHwnd, UINT _pMsg, WPARAM _pWParam, LPARAM _pLParam)
    {
        switch (_pMsg) 
        { 
        case WM_CREATE: 
            ///< Initialize the window. 
            return 0; 
 
        case WM_PAINT: 
            ///< Paint the window's client area. 
            return 0; 
 
        case WM_SIZE:
            glViewport( 0, 0, LOWORD(_pLParam), HIWORD(_pLParam));
            ///< Set the size and position of the window. 
            return 0;

        case WM_KEYDOWN: 
            switch (_pWParam) 
            { 
            case VK_LEFT: 
                    
                ///< Process the LEFT ARROW key. 
                     
                break; 
 
            case VK_RIGHT: 
                    
                ///< Process the RIGHT ARROW key. 
                     
                break; 
 
            case VK_UP:
                DestroyWindow(_pHwnd);
                ///< Process the UP ARROW key. 
                     
                break; 
 
            case VK_DOWN: 
                    
                ///< Process the DOWN ARROW key. 
                     
                break; 
 
            case VK_HOME: 
                    
                ///< Process the HOME key. 
                     
                break; 
 
            case VK_END: 
                    
                ///< Process the END key. 
                     
                break; 
 
            case VK_INSERT: 
                    
                ///< Process the INS key. 
                     
                break; 
 
            case VK_DELETE: 
                    
                ///< Process the DEL key. 
                     
                break; 
 
            case VK_F2: 
                    
                ///< Process the F2 key. 
                    
                break;
            
            default:
                break;
            }
            break;      

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
                
            // 
            // Process other messages. 
            // 
 
        default: 
            return DefWindowProc(_pHwnd, _pMsg, _pWParam, _pLParam);
        }
        return 0;
    }
}
