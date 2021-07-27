#include "IWindow.h"
#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
#include "GLPointer.h"

namespace GLVM::Core
{
    
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042
#define WGL_TYPE_RGBA_ARB 0x202B
        
    class CWindowWin : public IWindow
    {
        
        WNDCLASS window_Class_;
        HWND pModern_Window_;
        HDC pModern_DC_;
        HGLRC pModern_Context_;
        
    public:
        CWindowWin();

        void SwapBuffers() override;

        void ClearDisplay() override;
    
        void HandleEvent(CEvent& _Event) override;

        void Close() override;
            
        ///< Callback method for events handling.
        static LRESULT CALLBACK MainWndProc(HWND _pHwnd, UINT _pMsg, WPARAM _pWParam, LPARAM _pLParam);
    }; // namespace GLVM::Core
}    
