#include "UnixApi/WindowXCBVulkan.hpp"
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <cstdint>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xfixes.h>
#include <xcb/xproto.h>

namespace GLVM::core
{
	// static void testCookie (xcb_void_cookie_t cookie,
    //             xcb_connection_t *connection,
    //             const char *errMessage )
    // {   
    //     xcb_generic_error_t *error = xcb_request_check (connection, cookie);
    //     if (error) {
    //         fprintf (stderr, "ERROR: %s : %i\n", errMessage , error->error_code);
    //         xcb_disconnect (connection);
    //         exit (-1);
    //     }   
    // }   
	
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
//		event_flags[2] = cursor;
		
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

		// Cursor cursor;
		// Pixmap bitmapNoData;
        // XColor black;
        // static char noData[] = { 0,0,0,0,0,0,0,0 };
        // black.red = black.green = black.blue = 0;

		// xcb_pixmap_t pixmap = xcb_generate_id ( connection );
		
		// xcb_void_cookie_t cookie = xcb_create_pixmap(connection, 1, pixmap, window, 8, 8);



		/// Make sure commands are sent befour we pause so that the window gets shown
		xcb_flush ( connection );

		xcb_xfixes_query_version(connection, 4, 0);
		xcb_xfixes_hide_cursor(connection, window);

		xcb_flush ( connection );


		// // Grab the pointer and hide the cursor
		// xcb_grab_pointer_cookie_t grabCookie = xcb_grab_pointer(
		// 	connection,  // Connection
		// 	0,           // Owner Events
		// 	window,  // Grab Window
		// 	XCB_NONE,    // Event Mask
		// 	XCB_GRAB_MODE_ASYNC,  // Pointer Mode
		// 	XCB_GRAB_MODE_ASYNC,  // Keyboard Mode
		// 	window,  // Confine To
		// 	XCB_NONE,    // Cursor
		// 	XCB_CURRENT_TIME  // Time
		// 	);

		// // Grab the pointer reply
		// xcb_grab_pointer_reply_t* grabReply = xcb_grab_pointer_reply(connection, grabCookie, NULL);

		// if (grabReply && grabReply->status == XCB_GRAB_STATUS_SUCCESS) {
		// 	// Cursor hidden successfully
		// 	// Do your app logic here
		// }

		// // Free the grab reply
		// free(grabReply);



		// // Change the cursor attribute of the root window to None
		// uint32_t xcb_none = XCB_NONE;
		// xcb_change_window_attributes(connection, window, XCB_CW_CURSOR, &xcb_none);

		// Flush the request and wait for the server to process it
		xcb_flush(connection);
	   
		
		// uint32_t cursor = xcb_generate_id (connection);
		// xcb_create_glyph_cursor (connection,
		// 						 cursor,            /* cursor id  */
		// 						 XCB_NONE,       /* source glyph font */
		// 						 XCB_NONE,         /* mask glyph font */
		// 						 0,                /* source character glyph */
		// 						 0,            /* mask character glyph */
		// 						 0, 0, 0, 0, 0, 0); /* r b g r b g */

		// uint32_t mask       = XCB_CW_CURSOR;
        // uint32_t value_list = cursor;
        // xcb_change_window_attributes (connection, window, mask, &value_list);

		// xcb_flush ( connection );


		
		// xcb_font_t font = xcb_generate_id (connection);
        // xcb_void_cookie_t fontCookie = xcb_open_font_checked (connection,
        //                                                       font,
        //                                                       7,
        //                                                       "cursor" );
        // testCookie (fontCookie, connection, "can't open font");

		// uint32_t mask_for_cursor = 10;
        // xcb_cursor_t cursor = xcb_generate_id (connection);
        // xcb_create_glyph_cursor (connection,
        //                          cursor,
        //                          font,
        //                          font,
        //                          mask_for_cursor,
        //                          mask_for_cursor + 1,
        //                          0, 0, 0, 0, 0, 0 );

        // xcb_gcontext_t gc = xcb_generate_id (connection);

        // uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
        // uint32_t values_list[3];
        // values_list[0] = screen->black_pixel;
        // values_list[1] = screen->white_pixel;
        // values_list[2] = font;

		// xcb_void_cookie_t gcCookie = xcb_create_gc_checked (connection, gc, window, mask, values_list);
        // testCookie (gcCookie, connection, "can't create gc");

        // mask = XCB_CW_CURSOR;
        // uint32_t value_list = cursor;
        // xcb_change_window_attributes (connection, window, mask, &value_list);

        // xcb_free_cursor (connection, cursor);

        // fontCookie = xcb_close_font_checked (connection, font);
        // testCookie (fontCookie, connection, "can't close font");


		
 		// xcb_grab_pointer(connection, 1, window, XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_PRESS,
		// 				 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, window, XCB_NONE, XCB_CURRENT_TIME);
		
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
				case 1:
					_Event.SetEvent(EEvents::eMOUSE_LEFT_BUTTON);
					printf ("Button %d pressed in window %i, at coordinates (%d,%d)\n",
							expose_event->detail, expose_event->event, expose_event->event_x, expose_event->event_y);
					break;
				case 3:
					_Event.SetEvent(EEvents::eMOUSE_RIGHT_BUTTON);
					printf ("Button %d pressed in window %i, at coordinates (%d,%d)\n",
							expose_event->detail, expose_event->event, expose_event->event_x, expose_event->event_y);
					break;
				case 4:
					printf ("Wheel Button up in window %i, at coordinates (%d,%d)\n",
							expose_event->event, expose_event->event_x, expose_event->event_y);
					break;
				case 5:
					printf ("Wheel Button down in window %i, at coordinates (%d,%d)\n",
							expose_event->event, expose_event->event_x, expose_event->event_y);
					break;
				}
				
				break;
			}
			case XCB_BUTTON_RELEASE: {
				xcb_button_release_event_t *expose_event = (xcb_button_release_event_t *)generic_event;
				print_modifiers(expose_event->state);

				switch (expose_event->detail) {
				case 1:
					_Event.SetEvent(EEvents::eMOUSE_LEFT_BUTTON_RELEASE);
					printf ("Button %d released in window %i, at coordinates (%d,%d)\n",
							expose_event->detail, expose_event->event, expose_event->event_x, expose_event->event_y);
					break;
				case 3:
					_Event.SetEvent(EEvents::eMOUSE_RIGHT_BUTTON_RELEASE);
					printf ("Button %d released in window %i, at coordinates (%d,%d)\n",
							expose_event->detail, expose_event->event, expose_event->event_x, expose_event->event_y);
					break;
				}
				
				break;
			}
			case XCB_MOTION_NOTIFY: {
				xcb_motion_notify_event_t *expose_event = (xcb_motion_notify_event_t *)generic_event;

				_Event.SetEvent(EEvents::eMOUSE_POINTER_POSITION);
                _Event.mousePointerPosition.position_X = expose_event->event_x;
                _Event.mousePointerPosition.position_Y = expose_event->event_y;
				
				printf ("Mouse moved in window %i, at coordinates (%d,%d)\n",
						expose_event->event, expose_event->event_x, expose_event->event_y);
				break;
			}
			case XCB_MAP_WINDOW: {
				std::cout << "MAP WINDOW" << std::endl;
		
				/// Make sure commands are sent befour we pause so that the window gets shown
				xcb_flush ( connection );
				
				xcb_grab_pointer(connection, 1, window, XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_PRESS,
                XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, window, XCB_NONE, XCB_CURRENT_TIME);
				break;
			}
			case XCB_ENTER_NOTIFY: {
				xcb_enter_notify_event_t *expose_event = (xcb_enter_notify_event_t *)generic_event;

				printf ("Mouse entered window %i, at coordinates (%d,%d)\n",
						expose_event->event, expose_event->event_x, expose_event->event_y);
				break;
			}
			// case XCB_LEAVE_NOTIFY: {
			// 	xcb_leave_notify_event_t *expose_event = (xcb_leave_notify_event_t *)generic_event;

			// 	printf ("Mouse left window %i, at coordinates (%d,%d)\n",
			// 			expose_event->event, expose_event->event_x, expose_event->event_y);
			// 	break;
			// }
			case XCB_KEY_PRESS: {
				xcb_key_press_event_t *expose_event = (xcb_key_press_event_t *)generic_event;
				print_modifiers(expose_event->state);

				printf ("Key pressed in window %i\n",
						expose_event->event);

//				xcb_keycode_t key_code = expose_event->detail;
				std::cout << "Detail: " << xcb_key_press_lookup_keysym(key_symbols, expose_event, 0) << std::endl;
				xcb_keysym_t keysym = xcb_key_press_lookup_keysym(key_symbols, expose_event, 0);
				
				switch(keysym)
					{
					case 65307:
						_Event.SetEvent(EEvents::eGAME_LOOP_KILL);
						break;
					case 97:
						_Event.SetEvent(EEvents::eMOVE_LEFT);
						break;
					case 100:
						_Event.SetEvent(EEvents::eMOVE_RIGHT);
						break;
					case 115:
						_Event.SetEvent(EEvents::eMOVE_BACKWARD);
						break;
					case 119:
						_Event.SetEvent(EEvents::eMOVE_FORWARD);
						break;
					case 32:
						_Event.SetEvent(EEvents::eJUMP);
						break;
					}
				
				break;
			}
			case XCB_KEY_RELEASE: {
				xcb_key_release_event_t *key_release_event = (xcb_key_release_event_t *)generic_event;
				print_modifiers(key_release_event->state);

				printf ("Key released in window %i\n",
						key_release_event->event);

				xcb_generic_event_t* next_generic_event = xcb_poll_for_queued_event(connection);
				if ( next_generic_event != NULL ) {
					xcb_key_press_event_t *key_press_event = (xcb_key_press_event_t *)generic_event;
					xcb_keysym_t press_keysym = xcb_key_press_lookup_keysym(key_symbols, key_press_event, 0);
					xcb_keysym_t release_keysym = xcb_key_press_lookup_keysym(key_symbols, key_release_event, 0);
					
					if (next_generic_event->response_type == XCB_KEY_PRESS &&
						key_press_event->time == key_release_event->time &&
						press_keysym == release_keysym)
					{
						///< Key wasn’t actually released
						generic_event = xcb_poll_for_event (connection);
						continue;
					}
				}
				
				xcb_keysym_t release_keysym = xcb_key_press_lookup_keysym(key_symbols, key_release_event, 0);
				std::cout << "KEYSYM RELEASE: " << release_keysym << std::endl;
                switch(release_keysym)
                {
                case 97:
                    _Event.SetEvent(GLVM::core::eKEYRELEASE_A);
                    break;
                case 100:
                    _Event.SetEvent(GLVM::core::eKEYRELEASE_D);
                    break;
                case 115:
                    _Event.SetEvent(GLVM::core::eKEYRELEASE_S);
                    break;
                case 119:
                    _Event.SetEvent(GLVM::core::eKEYRELEASE_W);
                    break;
                case 32:
                    _Event.SetEvent(GLVM::core::eKEYRELEASE_JUMP);
                    break;
                }

				break;
			}
			// default:
			// 	/* Unknown event type, ignore it */
			// 	printf("Unknown event: %d\n", generic_event->response_type);
			// 	break;
			}
			/* Free the Generic Event */
			free (generic_event);
			return true;
		}

		return false;
	};
	
	void WindowXCBVulkan::Close() {};
	void WindowXCBVulkan::CursorLock([[maybe_unused]] int _x_position, [[maybe_unused]] int _y_position, [[maybe_unused]] int* _x_offset, [[maybe_unused]] int* _y_offset) {
		int iOffset_X = 0, iOffset_Y = 0;
        iOffset_X = _x_position - 960;
        iOffset_Y = _y_position - 540;
        
        *_x_offset += iOffset_X;
        *_y_offset -= iOffset_Y;

        if(*_y_offset > 890)
            *_y_offset = 890;
        else if(*_y_offset < -890)
            *_y_offset = -890;

		xcb_warp_pointer(connection, XCB_NONE, window, 0, 0, 0, 0, 960, 540);
		xcb_flush(connection);
	};
} // namespace GLVM::core
