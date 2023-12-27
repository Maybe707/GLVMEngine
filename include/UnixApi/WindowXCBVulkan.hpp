#ifndef WINDOW_XCB_VULKAN
#define WINDOW_XCB_VULKAN

#include <cstdint>
#include <unistd.h>

#include <xcb/xcb.h>

namespace GLVM::core
{
    typedef uint32_t xcb_window_t;
	
	class WindowXCBVulkan {
		xcb_connection_t* connection;
		xcb_window_t window;
		
	public:
		WindowXCBVulkan ();
		xcb_connection_t* GetConnection();
		xcb_window_t GetWindow();
		void Disconnect();
	};
} // namespace GLVM::core

#endif
