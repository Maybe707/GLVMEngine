#ifndef WINDOW_XCB_OPENGL
#define WINDOW_XCB_OPENGL

#include "IWindow.hpp"
#include <cstdint>
#include <unistd.h>
#include <cstdio>

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib-xcb.h> /* for XGetXCBConnection, link with libX11-xcb */
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_cursor.h>
#include <cassert>
#include <iostream>
#include "GLPointer.h"

namespace GLVM::core
{
    typedef uint32_t xcb_window_t;
	
	class WindowXCBOpengl : public IWindow
	{
		xcb_connection_t*  connection;
		xcb_screen_t*      screen;
		xcb_window_t       window;
		xcb_key_symbols_t* key_symbols;

		Display* display;
		int default_screen;
		GLXDrawable drawable;
		GLXContext context;
		GLXFBConfig fb_config;
		GLXContext (*pGLXCreateContextAttribsARB_) (Display*, GLXFBConfig,
													GLXContext, Bool, const int*) = 0;
		
		static void print_modifiers (uint32_t mask);
	public:
		WindowXCBOpengl ();

		int main_loop(Display *display, xcb_connection_t *connection, xcb_window_t window, GLXDrawable drawable);
		void setup_and_run(Display* display, xcb_connection_t *connection, int default_screen, xcb_screen_t *screen);
		void HideCursor();
		xcb_connection_t* GetConnection();
		xcb_window_t GetWindow();
		Display* GetDisplay();
		void Disconnect();

		void SwapBuffers() override;
        void ClearDisplay() override;
        bool HandleEvent(CEvent& _Event) override;
        void Close() override;
        void CursorLock(int _x_position, int _y_position, int* _x_offset, int* _y_offset) override;
	};
} // namespace GLVM::core

#endif
