#include "UnixApi/WindowXCBVulkan.hpp"
#include <X11/XKBlib.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

namespace GLVM::core
{
	WindowXCBVulkan::WindowXCBVulkan() {
		/// Open the connection to the X server
		connection = xcb_connect ( NULL, NULL );

		/// Get the first screen
		const xcb_setup_t*    setup    = xcb_get_setup ( connection );
		assert( connection != NULL);
		
		xcb_screen_iterator_t iterator = xcb_setup_roots_iterator ( setup );
		screen                         = iterator.data;

		key_symbols = xcb_key_symbols_alloc(connection);
		assert(key_symbols != NULL);
		
		uint32_t event_mask = 0;
		event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		uint32_t event_flags[2];
		event_flags[0] = screen->black_pixel;
		event_flags[1] = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
			XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE |
			XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW;
		
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
							event_mask, event_flags );                       ///< Masks, not used yet

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

	void WindowXCBVulkan::SwapBuffers() {};
	void WindowXCBVulkan::ClearDisplay() {};

	void WindowXCBVulkan::print_modifiers (uint32_t mask)
	{
		const char **mod, *mods[] = {
			"Shift", "Lock", "Ctrl", "Alt",
			"Mod2", "Mod3", "Mod4", "Mod5",
			"Button1", "Button2", "Button3", "Button4", "Button5"
		};
		printf ("Modifier mask: ");
		for (mod = mods ; mask; mask >>= 1, mod++)
			if (mask & 1)
				std::cout << *mod << std::endl;;
		putchar ('\n');
	}

	
	bool WindowXCBVulkan::HandleEvent([[maybe_unused]] CEvent& _Event) {
		xcb_generic_event_t* generic_event;

// 		while (( event = xcb_poll_for_event ( GetConnection() ))) {
// //			std::cout << event->response_type << std::endl;
// 		}

		while ((generic_event = xcb_poll_for_event (connection))) {
			switch (generic_event->response_type & ~0x80) {
			case XCB_EXPOSE: {
				xcb_expose_event_t *expose_event = (xcb_expose_event_t *)generic_event;

				printf ("Window %i exposed. Region to be redrawn at location (%d,%d), with dimension (%d,%d)\n",
						expose_event->window, expose_event->x, expose_event->y, expose_event->width, expose_event->height);
				break;
			}
			case XCB_BUTTON_PRESS: {
				xcb_button_press_event_t *expose_event = (xcb_button_press_event_t *)generic_event;
				print_modifiers(expose_event->state);

				switch (expose_event->detail) {
				case 4:
					printf ("Wheel Button up in window %i, at coordinates (%d,%d)\n",
							expose_event->event, expose_event->event_x, expose_event->event_y);
					break;
				case 5:
					printf ("Wheel Button down in window %i, at coordinates (%d,%d)\n",
							expose_event->event, expose_event->event_x, expose_event->event_y);
					break;
				default:
					printf ("Button %d pressed in window %i, at coordinates (%d,%d)\n",
							expose_event->detail, expose_event->event, expose_event->event_x, expose_event->event_y);
				}
				break;
			}
			case XCB_BUTTON_RELEASE: {
				xcb_button_release_event_t *expose_event = (xcb_button_release_event_t *)generic_event;
				print_modifiers(expose_event->state);

				printf ("Button %d released in window %i, at coordinates (%d,%d)\n",
						expose_event->detail, expose_event->event, expose_event->event_x, expose_event->event_y);
				break;
			}
			case XCB_MOTION_NOTIFY: {
				xcb_motion_notify_event_t *expose_event = (xcb_motion_notify_event_t *)generic_event;

				printf ("Mouse moved in window %i, at coordinates (%d,%d)\n",
						expose_event->event, expose_event->event_x, expose_event->event_y);
				break;
			}
			case XCB_ENTER_NOTIFY: {
				xcb_enter_notify_event_t *expose_event = (xcb_enter_notify_event_t *)generic_event;

				printf ("Mouse entered window %i, at coordinates (%d,%d)\n",
						expose_event->event, expose_event->event_x, expose_event->event_y);
				break;
			}
			case XCB_LEAVE_NOTIFY: {
				xcb_leave_notify_event_t *expose_event = (xcb_leave_notify_event_t *)generic_event;

				printf ("Mouse left window %i, at coordinates (%d,%d)\n",
						expose_event->event, expose_event->event_x, expose_event->event_y);
				break;
			}
			case XCB_KEY_PRESS: {
				xcb_key_press_event_t *expose_event = (xcb_key_press_event_t *)generic_event;
				print_modifiers(expose_event->state);

				printf ("Key pressed in window %i\n",
						expose_event->event);

//				xcb_keycode_t key_code = expose_event->detail;
				std::cout << "Detail: " << xcb_key_press_lookup_keysym(key_symbols, expose_event, 0) << std::endl;
				
				switch ((int)expose_event->event) {
				case (int)('&'):
					_Event.SetEvent(EEvents::eMOVE_LEFT);
					return true;
					break;
				}
				break;
			}
			case XCB_KEY_RELEASE: {
				xcb_key_release_event_t *expose_event = (xcb_key_release_event_t *)generic_event;
				print_modifiers(expose_event->state);

				printf ("Key released in window %i\n",
						expose_event->event);
				break;
			}
			default:
				/* Unknown event type, ignore it */
				printf("Unknown event: %d\n", generic_event->response_type);
				break;
			}
			/* Free the Generic Event */
			free (generic_event);
		}

		return false;
	};
	
	void WindowXCBVulkan::Close() {};
	void WindowXCBVulkan::CursorLock([[maybe_unused]] int _x_position, [[maybe_unused]] int _y_position, [[maybe_unused]] int* _x_offset, [[maybe_unused]] int* _y_offset) {};
} // namespace GLVM::core
