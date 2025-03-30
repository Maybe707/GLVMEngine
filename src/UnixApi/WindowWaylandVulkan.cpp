#include "UnixApi/WindowWaylandVulkan.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "UnixApi/pointer-constraints-unstable-v1-client-protocol.h"
#include <vulkan/vulkan_core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

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
		pointer_surface = wl_compositor_create_surface(compositor);
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

		// if (!seat || !pointer_constraints || !relative_pointer_manager || !pointer) {
		// 	fprintf(stderr, "Missing required Wayland globals\n");
		// 	exit(1);
		// }
		

		std::cout << "CONSTRUCTOR WAYLAND" << std::endl;
	}

	bool WindowWaylandVulkan::HandleEvent([[maybe_unused]] CEvent& _Event) {
		/* Process events and dispatch them to the appropriate Wayland objects, such as surfaces,
		   buffers, and other resources.
		*/
//		_Event.SetEvent(EEvents::eMOUSE_POINTER_POSITION);
		_Event.mousePointerPosition.position_X = x_pointer;
		_Event.mousePointerPosition.position_Y = y_pointer;
		x_pointer = 0;
		y_pointer = 0;

		wl_display_dispatch( display );
		if ( keys_pressed[0] == 23 )
			_Event.SetEvent(EEvents::eINVENTORY);			
// 		while (wl_display_dispatch( display )) {
// //			printf("%s", "HREN GOVNA!");
// 			if ( close_xdg_toplevel )
// 				break;
// 		}
		return false;
	}

// Create transparent cursor
	struct wl_buffer* WindowWaylandVulkan::create_transparent_cursor([[maybe_unused]] struct wl_shm *shm) {
		int size = 4 * 64 * 64; // 64x64 RGBA cursor (common size)
		int32_t file_descriptor = alocate_shared_memory( size );
		void* data = mmap(NULL, width * height * 4, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
    
		// Fill with transparent pixels
		for ( int i = 0; i < 64 * 64; ++i )
			((int*)data)[i] = 0x00000000;

		struct wl_shm_pool *pool = wl_shm_create_pool(shm, file_descriptor, size);
		struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, 64, 64, 64*4, WL_SHM_FORMAT_ARGB8888);
    
		munmap(data, size);
		close(file_descriptor);
		wl_shm_pool_destroy(pool);
    
		return buffer;
	}
	
	void WindowWaylandVulkan::SwapBuffers() {
	};
	void WindowWaylandVulkan::ClearDisplay() {
	};
	void WindowWaylandVulkan::CursorLock([[maybe_unused]] int _x_position, [[maybe_unused]] int _y_position, [[maybe_unused]] int* _x_offset, [[maybe_unused]] int* _y_offset) {
        // *_x_offset = _x_position - previous_X;
		// previous_X += *_x_offset;

		// *_y_offset = _y_position - previous_Y;
		// previous_Y += *_y_offset;

		static int flag = 0;
		if ( flag == 0 ) {
			*_x_offset = -960;
			*_y_offset = -540;
			++flag;
		} else {
			*_x_offset = _x_position;
			*_y_offset = _y_position;
		}
		
        // *_x_offset = _x_position + previous_X;
		// previous_X = *_x_offset;

		// *_y_offset = _y_position + previous_Y;
		// previous_Y = *_y_offset;
		
		// *_x_offset = _x_position;
		// *_y_offset = _y_position;
		// std::cout << "x offset: " << *_x_offset << std::endl;
		// std::cout << "y offset: " << *_y_offset << std::endl;
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
		// if ( key == 1 ) {
		// 	close_xdg_toplevel = 1;
		// } else if ( key == 30 ) {
		// 	printf("a\n");
		// } else if ( key == 32 ) {
		// 	printf("d\n");
		// }

		if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
			printf("Key pressed: %u\n", key);
			if (key == 1) {  // Typically ESC key
				printf("ESC pressed - exiting\n");
				g_eEvent.SetEvent(EEvents::eGAME_LOOP_KILL);
				Input_Stack_.ControlInput(g_eEvent);
				wl_display_disconnect(display);
			}
			if (key == 17) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eMOVE_FORWARD);
				Input_Stack_.ControlInput(g_eEvent);
				printf("w\n");
			}
			if (key == 31) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eMOVE_BACKWARD);
				Input_Stack_.ControlInput(g_eEvent);
				printf("s\n");
			}
			if (key == 30) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eMOVE_LEFT);
				Input_Stack_.ControlInput(g_eEvent);
				printf("a\n");
			}
			if (key == 32) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eMOVE_RIGHT);
				Input_Stack_.ControlInput(g_eEvent);
				printf("d\n");
			}
			if (key == 57) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eJUMP);
				Input_Stack_.ControlInput(g_eEvent);
				printf("space\n");
			}
			if (key == 23) {  // Typically ESC key
				// wl_events.WL_KEY_PRESSED_I = true;
				// keys_pressed[0] = 23;
				g_eEvent.SetEvent(EEvents::eINVENTORY);
				Input_Stack_.ControlInput(g_eEvent);
				printf("inventory\n");
			}
		}

		if (state == WL_KEYBOARD_KEY_STATE_RELEASED) {
			printf("Key pressed: %u\n", key);
			if (key == 1) {  // Typically ESC key
				printf("ESC pressed - exiting\n");
				wl_display_disconnect(display);
			}
			if (key == 17) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eKEYRELEASE_W);
				Input_Stack_.ControlInput(g_eEvent);
				printf("w\n");
			}
			if (key == 31) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eKEYRELEASE_S);
				Input_Stack_.ControlInput(g_eEvent);
				printf("s\n");
			}
			if (key == 30) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eKEYRELEASE_A);
				Input_Stack_.ControlInput(g_eEvent);
				printf("a\n");
			}
			if (key == 32) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eKEYRELEASE_D);
				Input_Stack_.ControlInput(g_eEvent);
				printf("d\n");
			}
			if (key == 57) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eKEYRELEASE_JUMP);
				Input_Stack_.ControlInput(g_eEvent);
				printf("space\n");
			}
			if (key == 23) {  // Typically ESC key
				g_eEvent.SetEvent(EEvents::eINVENTORY_RELEASE);
				Input_Stack_.ControlInput(g_eEvent);
//				keys_pressed[0] = 23;
				printf("inventory\n");
			}
		}
	}

	void WindowWaylandVulkan::keyboard_modifiers([[maybe_unused]] void* data, [[maybe_unused]] struct wl_keyboard* keyboard, [[maybe_unused]] uint32_t serial,
												 [[maybe_unused]] uint32_t mods_depressed, [[maybe_unused]] uint32_t mods_latched, [[maybe_unused]] uint32_t mods_locked, [[maybe_unused]] uint32_t group) {
	
	}

	void WindowWaylandVulkan::keyboard_repeat_info([[maybe_unused]] void* data, [[maybe_unused]] struct wl_keyboard* keyboard, [[maybe_unused]] int32_t rate,
												   [[maybe_unused]] int32_t delay) {
	
	}

	// Pointer listener callbacks
	void WindowWaylandVulkan::pointer_enter([[maybe_unused]] void *data, [[maybe_unused]] struct wl_pointer *pointer,
							  [[maybe_unused]] uint32_t serial, [[maybe_unused]] struct wl_surface *surface,
							  wl_fixed_t sx, wl_fixed_t sy) {
		printf("Pointer entered surface at %f, %f\n",
			   wl_fixed_to_double(sx), wl_fixed_to_double(sy));

	}

	void WindowWaylandVulkan::pointer_leave([[maybe_unused]] void *data, [[maybe_unused]] struct wl_pointer *pointer,
							  [[maybe_unused]] uint32_t serial, [[maybe_unused]] struct wl_surface *surface) {
		printf("Pointer left surface\n");
	}
	
	void WindowWaylandVulkan::pointer_motion([[maybe_unused]] void *data, [[maybe_unused]] struct wl_pointer *pointer,
							   [[maybe_unused]] uint32_t time, [[maybe_unused]] wl_fixed_t sx, [[maybe_unused]] wl_fixed_t sy) {
		// printf("Pointer moved to %f, %f\n",
		// 	   wl_fixed_to_double(sx), wl_fixed_to_double(sy));
		// x_pointer = wl_fixed_to_int(sx);
		// y_pointer = wl_fixed_to_int(sy);
	}

	void WindowWaylandVulkan::pointer_axis([[maybe_unused]] void *data, [[maybe_unused]] struct wl_pointer *pointer,
							 [[maybe_unused]] uint32_t time, uint32_t axis, wl_fixed_t value) {
		const char *axis_name = axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL ? 
			"horizontal" : "vertical";
		printf("Scroll %s by %f\n", axis_name, wl_fixed_to_double(value));
	}

	bool WindowWaylandVulkan::hideAndLockPointer = false;
	
	void WindowWaylandVulkan::pointer_button([[maybe_unused]] void *data, [[maybe_unused]] struct wl_pointer *pointer,
							   [[maybe_unused]] uint32_t serial, [[maybe_unused]] uint32_t time, uint32_t button,
							   uint32_t state) {
		const char *button_name = "unknown";
		std::cout << "button number: " << button << std::endl;
		// switch (button) {
        // case BTN_LEFT: button_name = "left"; break;
        // case BTN_RIGHT: button_name = "right"; break;
        // case BTN_MIDDLE: button_name = "middle"; break;
		// }

		if ( state == WL_POINTER_BUTTON_STATE_PRESSED ) {
			if ( button == 272 ) {
				g_eEvent.SetEvent(EEvents::eMOUSE_LEFT_BUTTON);
				Input_Stack_.ControlInput(g_eEvent);
				g_eEvent.isLeftMouseButtonReleased = true;
			}
		}
		if ( state == WL_POINTER_BUTTON_STATE_RELEASED ) {
			if ( button == 272 ) {
				g_eEvent.SetEvent(EEvents::eMOUSE_LEFT_BUTTON_RELEASE);
				Input_Stack_.ControlInput(g_eEvent);
			}
		}
		
//        wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
//		pointer = nullptr;

		
		// Hide cursor on first opportunity
		if ( !hideAndLockPointer ) {
			hideAndLockPointer = true;
			struct wl_buffer *transparent = create_transparent_cursor(pointer_shared_memory);
			wl_surface_attach(pointer_surface, transparent, 0, 0);
			wl_surface_commit(pointer_surface);
			wl_pointer_set_cursor(pointer, serial, pointer_surface, 0, 0);

			if (!pointer_constraints) {
				printf("Pointer constraints not available!\n");
				return;
			}

			// Lock pointer to main window surface, not pointer_surface
			[[maybe_unused]] zwp_locked_pointer_v1* locked_pointer = zwp_pointer_constraints_v1_lock_pointer(
				pointer_constraints,
				wl_surface,  // Use main window surface
				pointer,
				NULL,
				ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT);

			// get relative motion
			relative_pointer = zwp_relative_pointer_manager_v1_get_relative_pointer(
				relative_pointer_manager, pointer);
			zwp_relative_pointer_v1_add_listener(relative_pointer, &relative_pointer_listener, NULL);

		
			// // lock the pointer
			// [[maybe_unused]] zwp_locked_pointer_v1 *locked_pointer = zwp_pointer_constraints_v1_lock_pointer(pointer_constraints, pointer_surface, pointer, NULL,
			// 										ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT);
		}
		
		printf("%s mouse button %s\n",
			   state == WL_POINTER_BUTTON_STATE_PRESSED ? "Pressed" : "Released",
			   button_name);
	}

	void WindowWaylandVulkan::handle_relative_motion([[maybe_unused]] void *data, [[maybe_unused]] struct zwp_relative_pointer_v1 *rel_pointer, [[maybe_unused]] uint32_t utime_hi, [[maybe_unused]] uint32_t utime_lo,
													 wl_fixed_t dx, wl_fixed_t dy, [[maybe_unused]] wl_fixed_t dx_unaccel, [[maybe_unused]] wl_fixed_t dy_unaccel) {
		printf("Relative motion: dx=%.2f dy=%.2f\n",
			   wl_fixed_to_double(dx), wl_fixed_to_double(dy));
		x_pointer = wl_fixed_to_int(dx);
		y_pointer = wl_fixed_to_int(dy);
	}

	struct zwp_relative_pointer_v1_listener WindowWaylandVulkan::relative_pointer_listener = {
		.relative_motion = handle_relative_motion
	};
	
	struct wl_pointer_listener WindowWaylandVulkan::pointer_listener = {
		.enter = pointer_enter,
		.leave = pointer_leave,
		.motion = pointer_motion,
		.button = pointer_button,
		.axis = pointer_axis,
		.frame = nullptr,
		.axis_source = nullptr,
		.axis_stop = nullptr,
		.axis_discrete = nullptr,
		.axis_value120 = nullptr,
		.axis_relative_direction = nullptr
	};

	void WindowWaylandVulkan::seat_capabilities( [[maybe_unused]] void* data, struct wl_seat* seat, uint32_t capabilities ) {
		// Handle pointer capabilities
		if ((capabilities & WL_SEAT_CAPABILITY_POINTER) && !pointer) {
			pointer = wl_seat_get_pointer(seat);
			wl_pointer_add_listener(pointer, &pointer_listener, NULL);
		} else if (!(capabilities & WL_SEAT_CAPABILITY_POINTER) && pointer) {
			wl_pointer_destroy(pointer);
			pointer = NULL;
		}
		
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
			pointer_shared_memory = (wl_shm*)wl_registry_bind( registry, name, &wl_shm_interface, 1 );
		} else if (!strcmp( interface, zwp_pointer_constraints_v1_interface.name )) {
			pointer_constraints = (zwp_pointer_constraints_v1*)wl_registry_bind(registry, name, &zwp_pointer_constraints_v1_interface, 1);
		} else if (!strcmp( interface, zwp_relative_pointer_manager_v1_interface.name)) {
			relative_pointer_manager = (zwp_relative_pointer_manager_v1*)wl_registry_bind(registry, name, &zwp_relative_pointer_manager_v1_interface, 1);
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

