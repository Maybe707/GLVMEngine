#include "UnixApi/WindowWaylandVulkan.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "UnixApi/xdg-shell-client-protocol.h"
#include <cstddef>

namespace GLVM::core {
	WindowWaylandVulkan::WindowWaylandVulkan() {
		// connects your client application to the Wayland display server
		display  = wl_display_connect(0);               
		/* get the global registry object from the Wayland display server (compositor). This registry allows
		   the client to discover available global objects, such as wl_compositor, wl_shm, xdg_wm_base, etc.,
		   which are needed to create surfaces and interact with the window system.
		*/
		registry = wl_display_get_registry(display);
		/* used to attach a listener (callback functions) to the Wayland registry object (wl_registry) so
		   that your client can respond to announcements about global objects provided by the compositor
		*/
		wl_registry_add_listener( registry, &registry_listener, 0 );
		/* synchronize the client with the Wayland compositor
		   1. global object announcements (e.g., from wl_registry)
		   2. event responses to previously sent requests
		*/
		wl_display_roundtrip( display );
		if (!compositor || !xdg_shell) {
			fprintf(stderr, "Error: compositor or xdg_shell is NULL!\n");
			exit(1);
		}
		
		/* create a new surface — which is essentially a drawable area in the Wayland compositor. This
		   surface becomes the foundation for windows, popups, and anything visual in a Wayland client
		*/
		wl_surface = wl_compositor_create_surface( compositor );
		/* set up a frame callback, which lets the client know when it's a good time to start rendering
		   the next frame — usually tied to the compositor's refresh cycle (like vsync).
		*/
		frame_callback = wl_surface_frame( wl_surface );
		// attach a listener (callback function) to a wl_callback object — typically created using wl_surface_frame
		wl_callback_add_listener( frame_callback, &callback_listener, 0 );

		/* create a top-level window or popup window from a given wl_surface.
		   wraps a wl_surface with an XDG surface, which provides window management features
		*/
		xdg_surface = xdg_wm_base_get_xdg_surface( xdg_shell, wl_surface );
		/* Attach event handlers (callbacks) to an xdg_surface so your application can respond to events
		   from the compositor. When something happens to this surface (like resize, configure, etc.),
		   please call these functions.
		*/
		xdg_surface_add_listener( xdg_surface, &xdg_surface_listener, 0 );
		/* Turn a basic xdg_surface into a toplevel window — like a normal app window with borders,
		   title bar, and so on.
		*/
		xdg_topLevel = xdg_surface_get_toplevel( xdg_surface );
		xdg_toplevel_add_listener( xdg_topLevel, &xdg_toplevel_listener, 0 );
		xdg_toplevel_set_title( xdg_topLevel, "wayland glvm client" );
		/* Commit the changes made to a Wayland surface, notifying the compositor to render those
		   changes to the screen.
		*/
		wl_surface_commit( wl_surface );

		/* Process events and dispatch them to the appropriate Wayland objects, such as surfaces,
		   buffers, and other resources.
		*/

		// if ( buffer == NULL )
		// 	std::cout << "BUFFER SOVSEM V GOVNE" << std::endl;
		// else
		// 	std::cout << "BUFFER V PORYADE" << std::endl;
		
		// while (wl_display_dispatch( display )) {
		// 	if ( wl_surface == NULL )
		// 		std::cout << "NULL SURFACE" << std::endl;
		// 	else if ( compositor == NULL )
		// 		std::cout << "NULL COMPOSITOR" << std::endl;
		// 	else if ( xdg_topLevel == NULL )
		// 		std::cout << "NULL XDG_TOPLEVEL" << std::endl;
		// 	else if ( xdg_shell == NULL )
		// 		std::cout << "NULL XDG_SHELL" << std::endl;
		// 	else if ( buffer == NULL )
		// 		std::cout << "NULL BUFFER" << std::endl;
		// 	else if ( shared_memory == NULL )
		// 		std::cout << "NULL SHARED_MEMORY" << std::endl;
		// 	else if ( seat == NULL )
		// 		std::cout << "NULL SEAT" << std::endl;
		// 	else if ( keyboard == NULL )
		// 		std::cout << "NULL KEYBOARD" << std::endl;
		// 	else if ( display == NULL )
		// 		std::cout << "NULL DISPLAY" << std::endl;
		// 	else if ( registry == NULL )
		// 		std::cout << "NULL REGISTRY" << std::endl;
		// 	else if ( frame_callback == NULL )
		// 		std::cout << "NULL FRAME_CALLBACK" << std::endl;
		// 	else if ( xdg_surface == NULL )
		// 		std::cout << "NULL XDG_SURFACE" << std::endl;
		// 	else if ( pixels == NULL )
		// 		std::cout << "NULL PIXELS" << std::endl;
			
		// 	if ( close_xdg_toplevel )
		// 		break;
		// }
		// if ( buffer == NULL )
		// 	std::cout << "BUFFER SOVSEM V GOVNE" << std::endl;
		// else
		// 	std::cout << "BUFFER V PORYADE" << std::endl;
		std::cout << "CONSTRUCTOR WAYLAND" << std::endl;
	}

	bool WindowWaylandVulkan::HandleEvent([[maybe_unused]] CEvent& _Event) {
		// if ( wl_surface == NULL )
		// 	std::cout << "NULL SURFACE" << std::endl;
		// else if ( compositor == NULL )
		// 	std::cout << "NULL COMPOSITOR" << std::endl;
		// else if ( xdg_topLevel == NULL )
		// 	std::cout << "NULL XDG_TOPLEVEL" << std::endl;
		// else if ( xdg_shell == NULL )
		// 	std::cout << "NULL XDG_SHELL" << std::endl;
		// else if ( buffer == NULL )
		// 	std::cout << "NULL BUFFER" << std::endl;
		// else if ( shared_memory == NULL )
		// 	std::cout << "NULL SHARED_MEMORY" << std::endl;
		// else if ( seat == NULL )
		// 	std::cout << "NULL SEAT" << std::endl;
		// else if ( keyboard == NULL )
		// 	std::cout << "NULL KEYBOARD" << std::endl;
		// else if ( display == NULL )
		// 	std::cout << "NULL DISPLAY" << std::endl;
		// else if ( registry == NULL )
		// 	std::cout << "NULL REGISTRY" << std::endl;
		// else if ( frame_callback == NULL )
		// 	std::cout << "NULL FRAME_CALLBACK" << std::endl;
		// else if ( xdg_surface == NULL )
		// 	std::cout << "NULL XDG_SURFACE" << std::endl;
		// else if ( pixels == NULL )
		// 	std::cout << "NULL PIXELS" << std::endl;

		wl_display_dispatch( display );
		
// 		while (wl_display_dispatch( display )) {
// //			printf("%s", "HREN GOVNA!");
// 			if ( close_xdg_toplevel )
// 				break;
// 		}
		return false;
	}

	void WindowWaylandVulkan::SwapBuffers() {
	};
	void WindowWaylandVulkan::ClearDisplay() {
	};
	void WindowWaylandVulkan::CursorLock([[maybe_unused]] int _x_position, [[maybe_unused]] int _y_position, [[maybe_unused]] int* _x_offset, [[maybe_unused]] int* _y_offset) {
	};

	void WindowWaylandVulkan::Close() {
		if (keyboard) {
			wl_keyboard_destroy(keyboard);
		}
		/* Release a Wayland seat object, which is responsible for managing input devices like
		   keyboards, mice, or touchscreens.
		*/
		wl_seat_release( seat );
		if (buffer) {
			wl_buffer_destroy( buffer );
		}
		xdg_toplevel_destroy( xdg_topLevel );
		xdg_surface_destroy( xdg_surface );
		wl_surface_destroy( wl_surface );
		wl_display_disconnect( display );
	}
	
	int32_t WindowWaylandVulkan::alocate_shared_memory( uint64_t size ) {
		char name[8];
		name[0] = '/';
		name[7] = 0;
		for ( int8_t i = 1; i < 6; ++i ) {
			name[i] = ( rand() & 23 ) + 97;
		}
		/// shm_open, shm_unlink - create/open or unlink POSIX shared memory objects
		int32_t file_descriptor = shm_open( name, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
		shm_unlink( name );
		/// File truncation means cutting off a file at a certain size — either shrinking it or expanding it.
		[[maybe_unused]] int result = ftruncate( file_descriptor, size );

		return file_descriptor;
	}

	void WindowWaylandVulkan::resize() {
		int32_t file_descriptor = alocate_shared_memory( width * height * 4 );

		/*
		  1. addr: Preferred memory address where mapping should start.
		  2. length: how many bytes to map.
		  3. prot: memory protection (read/write/exec).
		  4. flags: type of mapping (private/shared/anonymous, etc.).
		  5. fd: file descriptor to map.
		  6. offset: offset into file (usually 0 to map from the beginning).

		  When you pass 0 (or NULL) as the first argument, you’re telling the kernel:
		  “I don’t care where you map the file in memory — just choose a suitable address for me.”
		*/
		pixels = mmap( 0, width * height * 4, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);

		/// Create a shared memory pool that clients can use to allocate memory buffers for drawing.
		struct wl_shm_pool* pool = wl_shm_create_pool( shared_memory, file_descriptor, width * height * 4 );
		/* It tells Wayland: “Take this part of the memory pool and treat it as an image buffer that
		   I’ll draw onto a window.”
		*/
		buffer = wl_shm_pool_create_buffer( pool, 0, width, height, width * 4, WL_SHM_FORMAT_ABGR8888 );
		// if ( buffer != NULL )
		// 	std::cout << "BUFFER NOT NULL" << std::endl;
		// else 
		// 	std::cout << "BUFFER IS NULL" << std::endl;
			
		wl_shm_pool_destroy( pool );
		close( file_descriptor );
	}

	void WindowWaylandVulkan::draw() {
		/// Fill a block of memory with a specific byte value.
		memset( pixels, constant_byte, width * height * 4 );

		/// Set a buffer as the content of this surface.
		wl_surface_attach( wl_surface, buffer, 0, 0 );
		/* Mark a specific area of a Wayland surface as "damaged", which means the
		   compositor should re-render or update that area.
		*/
		wl_surface_damage_buffer( wl_surface, 0, 0, width, height );
		/* Commit the changes you've made to a Wayland surface, sending them to the
		   compositor so they can be applied (i.e., rendered to the screen).
		*/

		wl_surface_commit( wl_surface );
	}

	void WindowWaylandVulkan::xdg_toplevel_configure( [[maybe_unused]] void* data, [[maybe_unused]] struct xdg_toplevel* xdg_toplevel, int32_t new_width, int32_t new_height, [[maybe_unused]] struct wl_array* atate ) {
		if ( !new_width && !new_height ) {
			return;
		}

		if ( width != new_width || height != new_height ) {
			munmap( pixels, width * new_height * 4 );
			width = new_width;
			height = new_height;
			resize();
		}
	}

	void WindowWaylandVulkan::xdg_toplevel_close( [[maybe_unused]] void* data, [[maybe_unused]] struct xdg_toplevel* xdg_toplevel ) {
		close_xdg_toplevel = 1;
	}

	void WindowWaylandVulkan::xdg_surface_configure( [[maybe_unused]] void* data, struct xdg_surface* xdg_surface, uint32_t serial ) {
		/* Acknowledge a configure event sent by the Wayland compositor to your xdg_surface
		   In Wayland, when the compositor wants to change your window (like resizing it), it sends
		   a configure event to your surface. You must call xdg_surface_ack_configure() to confirm
		   that you received and accepted this change. If you don’t call it, your window won’t be
		   shown or updated properly.
		*/
		xdg_surface_ack_configure( xdg_surface, serial );
		if ( !pixels ) {
			resize();
		}

		draw();
	}

	// uint16_t WindowWaylandVulkan::width  = 320;
	// uint16_t WindowWaylandVulkan::height = 240;
	// uint8_t  WindowWaylandVulkan::constant_byte = 0;

	struct wl_callback_listener WindowWaylandVulkan::callback_listener = {
		/* Notify the client when the related request is done.
		   param callback_data request-specific data for the callback
		*/
		.done = new_frame
	};
	struct wl_keyboard_listener WindowWaylandVulkan::keyboard_listener = {
		.keymap      = keyboard_keymap,
		.enter       = keyboard_enter,
		.leave       = keyboard_leave,
		.key         = keyboard_key,
		.modifiers   = keyboard_modifiers,
		.repeat_info = keyboard_repeat_info
	};
	struct xdg_wm_base_listener WindowWaylandVulkan::shell_listener = {
		.ping = shell_ping
	};
	struct wl_seat_listener WindowWaylandVulkan::seat_lintener = {
		.capabilities = seat_capabilities,
		.name         = seat_name
	};
	
	void WindowWaylandVulkan::new_frame( [[maybe_unused]] void* data, struct wl_callback* frame_call_back, [[maybe_unused]] uint32_t callback_data ) {
		wl_callback_destroy( frame_call_back );
		frame_call_back = wl_surface_frame( wl_surface );
		wl_callback_add_listener( frame_call_back, &callback_listener, 0 );

//	++constant_byte;
		draw();
	}

	void WindowWaylandVulkan::shell_ping( [[maybe_unused]] void* data, struct xdg_wm_base* shell, uint32_t serial ) {
		xdg_wm_base_pong( shell, serial );
	}


	void WindowWaylandVulkan::keyboard_keymap([[maybe_unused]] void* data, [[maybe_unused]] struct wl_keyboard* keyboard, [[maybe_unused]] uint32_t format,
											  [[maybe_unused]] int32_t keymap_file_descriptor, [[maybe_unused]] uint32_t size) {
	
	}

	void WindowWaylandVulkan::keyboard_enter([[maybe_unused]] void* data, [[maybe_unused]] struct wl_keyboard* keyboard, [[maybe_unused]] uint32_t serial,
											 [[maybe_unused]] struct wl_surface* surface, [[maybe_unused]] struct wl_array* keys) {
	
	}

	void WindowWaylandVulkan::keyboard_leave([[maybe_unused]] void* data, [[maybe_unused]] struct wl_keyboard* keyboard, [[maybe_unused]] uint32_t serial,
											 [[maybe_unused]] struct wl_surface* surface) {
	
	}

	void WindowWaylandVulkan::keyboard_key([[maybe_unused]] void* data, [[maybe_unused]] struct wl_keyboard* keyboard, [[maybe_unused]] uint32_t serial,
										   [[maybe_unused]] uint32_t time, uint32_t key, [[maybe_unused]] uint32_t state) {
		if ( key == 1 ) {
			close_xdg_toplevel = 1;
		} else if ( key == 30 ) {
			printf("a\n");
		} else if ( key == 32 ) {
			printf("d\n");
		}
	}

	void WindowWaylandVulkan::keyboard_modifiers([[maybe_unused]] void* data, [[maybe_unused]] struct wl_keyboard* keyboard, [[maybe_unused]] uint32_t serial,
												 [[maybe_unused]] uint32_t mods_depressed, [[maybe_unused]] uint32_t mods_latched, [[maybe_unused]] uint32_t mods_locked, [[maybe_unused]] uint32_t group) {
	
	}

	void WindowWaylandVulkan::keyboard_repeat_info([[maybe_unused]] void* data, [[maybe_unused]] struct wl_keyboard* keyboard, [[maybe_unused]] int32_t rate,
												   [[maybe_unused]] int32_t delay) {
	
	}


	void WindowWaylandVulkan::seat_capabilities( [[maybe_unused]] void* data, struct wl_seat* seat, uint32_t capabilities ) {
		if ( capabilities & WL_SEAT_CAPABILITY_KEYBOARD && !keyboard ) {
			keyboard = wl_seat_get_keyboard( seat );
			wl_keyboard_add_listener( keyboard, &keyboard_listener, 0 );
		}
	}

	void WindowWaylandVulkan::seat_name( [[maybe_unused]] void* data, [[maybe_unused]] struct wl_seat* seat, [[maybe_unused]] const char* name ) {
	}


	void WindowWaylandVulkan::registry_global( [[maybe_unused]] void* data, struct wl_registry* registry, uint32_t name, const char* interface, [[maybe_unused]] uint32_t version ) {
		if (!strcmp( interface, wl_compositor_interface.name )) {
			compositor = (wl_compositor*)wl_registry_bind( registry, name, &wl_compositor_interface, 4 );
		} else if (!strcmp( interface, wl_shm_interface.name )) {
			shared_memory = (wl_shm*)wl_registry_bind( registry, name, &wl_shm_interface, 1 );
		} else if (!strcmp( interface, xdg_wm_base_interface.name )) {
			xdg_shell = (xdg_wm_base*)wl_registry_bind( registry, name, &xdg_wm_base_interface, 1 );
			xdg_wm_base_add_listener( xdg_shell, &shell_listener, 0 );
		} else if (!strcmp( interface, wl_seat_interface.name )) {
			seat = (wl_seat*)wl_registry_bind( registry, name, &wl_seat_interface, 1 );
			wl_seat_add_listener( seat, &seat_lintener, 0 );
		}
	}

	void WindowWaylandVulkan::registry_global_remove( [[maybe_unused]] void* data, [[maybe_unused]] struct wl_registry* registry, [[maybe_unused]] uint32_t name) {
	}


}; ///< namespace GLVM::core

/* int main(int argc, char* argv[]) { */
/* 	// connects your client application to the Wayland display server */
/* 	struct wl_display*  display  = wl_display_connect(0);                */
/* 	/\* get the global registry object from the Wayland display server (compositor). This registry allows */
/* 	   the client to discover available global objects, such as wl_compositor, wl_shm, xdg_wm_base, etc., */
/* 	   which are needed to create surfaces and interact with the window system. */
/* 	*\/ */
/* 	struct wl_registry* registry = wl_display_get_registry(display); */
/* 	/\* used to attach a listener (callback functions) to the Wayland registry object (wl_registry) so */
/* 	   that your client can respond to announcements about global objects provided by the compositor */
/* 	*\/ */
/* 	wl_registry_add_listener( registry, &registry_listener, 0 ); */
/* 	/\* synchronize the client with the Wayland compositor */
/* 	   1. global object announcements (e.g., from wl_registry) */
/* 	   2. event responses to previously sent requests */
/* 	*\/ */
/* 	wl_display_roundtrip( display ); */

/* 	/\* create a new surface — which is essentially a drawable area in the Wayland compositor. This */
/* 	   surface becomes the foundation for windows, popups, and anything visual in a Wayland client */
/* 	*\/ */
/* 	surface = wl_compositor_create_surface( compositor ); */
/* 	/\* set up a frame callback, which lets the client know when it's a good time to start rendering */
/* 	   the next frame — usually tied to the compositor's refresh cycle (like vsync). */
/* 	*\/ */
/* 	struct wl_callback* frame_callback = wl_surface_frame( surface ); */
/* 	// attach a listener (callback function) to a wl_callback object — typically created using wl_surface_frame */
/* 	wl_callback_add_listener( frame_callback, &callback_listener, 0 ); */

/* 	/\* create a top-level window or popup window from a given wl_surface. */
/* 	   wraps a wl_surface with an XDG surface, which provides window management features */
/* 	*\/ */
/* 	struct xdg_surface* xdg_surface = xdg_wm_base_get_xdg_surface( xdg_shell, surface ); */
/* 	/\* Attach event handlers (callbacks) to an xdg_surface so your application can respond to events */
/* 	   from the compositor. When something happens to this surface (like resize, configure, etc.), */
/* 	   please call these functions. */
/* 	*\/ */
/* 	xdg_surface_add_listener( xdg_surface, &xdg_surface_listener, 0 ); */
/* 	/\* Turn a basic xdg_surface into a toplevel window — like a normal app window with borders, */
/* 	   title bar, and so on. */
/* 	 *\/ */
/* 	xdg_topLevel = xdg_surface_get_toplevel( xdg_surface ); */
/* 	xdg_toplevel_add_listener( xdg_topLevel, &xdg_toplevel_listener, 0 ); */
/* 	xdg_toplevel_set_title( xdg_topLevel, "wayland glvm client" ); */
/* 	/\* Commit the changes made to a Wayland surface, notifying the compositor to render those */
/* 	   changes to the screen. */
/* 	 *\/ */
/* 	wl_surface_commit( surface ); */

/* 	/\* Process events and dispatch them to the appropriate Wayland objects, such as surfaces, */
/* 	   buffers, and other resources. */
/* 	*\/ */
/* 	while (wl_display_dispatch( display )) { */
/* 		if ( close_xdg_toplevel ) */
/* 			break; */
/* 	} */

/* 	if (keyboard) { */
/* 		wl_keyboard_destroy(keyboard); */
/* 	} */
/* 	/\* Release a Wayland seat object, which is responsible for managing input devices like */
/* 	   keyboards, mice, or touchscreens. */
/* 	*\/ */
/* 	wl_seat_release( seat ); */
/* 	if (buffer) { */
/* 		wl_buffer_destroy( buffer ); */
/* 	} */
/* 	xdg_toplevel_destroy( xdg_topLevel ); */
/* 	xdg_surface_destroy( xdg_surface ); */
/* 	wl_surface_destroy( surface ); */
/* 	wl_display_disconnect( display ); */
	
/* 	return 0; */
/* } */
    
