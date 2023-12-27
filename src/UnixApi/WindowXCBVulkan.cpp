#include "UnixApi/WindowXCBVulkan.hpp"

namespace GLVM::core
{
	WindowXCBVulkan::WindowXCBVulkan() {
		/// Open the connection to the X server
		connection = xcb_connect ( NULL, NULL );

		/// Get the first screen
		const xcb_setup_t*    setup    = xcb_get_setup ( connection );
		xcb_screen_iterator_t iterator = xcb_setup_roots_iterator ( setup );
		xcb_screen_t*         screen   = iterator.data;

		/// Create window
		window = xcb_generate_id ( connection );
		xcb_create_window ( connection,                      ///< Connection
							XCB_COPY_FROM_PARENT,            ///< Depth (same as root)
							window,                          ///< Window id
							screen->root,                    ///< Parent window
							0, 0,                            ///< x, y
							640, 480,                        ///< width, height
							10,                              ///< Border width
							XCB_WINDOW_CLASS_INPUT_OUTPUT,   ///< Class
							screen->root_visual,             ///< Visual
							0, NULL );                       ///< Masks, not used yet

		/// Map the window on the screen
		xcb_map_window ( connection, window );

		/// Make sure commands are sent befour we pause so that the window gets shown
		xcb_flush ( connection );

//		pause ();      ///< Hold client until Ctrl-C

//		xcb_disconnect ( connection );
	}

	xcb_connection_t* WindowXCBVulkan::GetConnection() { return connection; }
	xcb_window_t WindowXCBVulkan::GetWindow() { return window; }
	void WindowXCBVulkan::Disconnect() { xcb_disconnect ( connection ); }
} // namespace GLVM::core
