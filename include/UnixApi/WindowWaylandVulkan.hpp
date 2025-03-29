#ifndef WINDOW_WAYLAND_VULKAN
#define WINDOW_WAYLAND_VULKAN

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include "UnixApi/xdg-shell-client-protocol.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>

#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "IWindow.hpp"
#include "Globals.hpp"

namespace GLVM::core {
	struct WindowWaylandVulkan : IWindow {
		WindowWaylandVulkan();
		void Close() override;
		bool HandleEvent(CEvent& _Event) override;
		static int create_anonymous_file(off_t size);
		static struct wl_buffer *create_transparent_cursor(struct wl_shm *shm);
		void SwapBuffers() override;
        void ClearDisplay() override;
        void CursorLock(int _x_position, int _y_position, int* _x_offset, int* _y_offset) override;
		static int32_t alocate_shared_memory( uint64_t size );
		static void resize();
		static void draw();
		static void xdg_toplevel_configure( void* data, struct xdg_toplevel* xdg_toplevel, int32_t new_width, int32_t new_height, struct wl_array* atate );
		static void xdg_toplevel_close( void* data, struct xdg_toplevel* xdg_toplevel );
		static void xdg_surface_configure( void* data, struct xdg_surface* xdg_surface, uint32_t serial );
		static void new_frame( void* data, struct wl_callback* frame_call_back, uint32_t callback_data );
		static void shell_ping( void* data, struct xdg_wm_base* shell, uint32_t serial );
		static void keyboard_keymap(void* data, struct wl_keyboard* keyboard, uint32_t format, int32_t keymap_file_descriptor, uint32_t size);
		static void keyboard_enter(void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys);
		static void keyboard_leave(void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface);
		static void keyboard_key(void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
		static void keyboard_modifiers(void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
		static void keyboard_repeat_info(void* data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay);
		static void pointer_enter(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy);
		static void pointer_leave(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
		static void pointer_motion(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy);
		static void pointer_axis(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
		static void pointer_button(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
		static void seat_capabilities( void* data, struct wl_seat* seat, uint32_t capabilities );
		static void seat_name( void* data, struct wl_seat* seat, const char* name );
		static void registry_global( void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version );
		static void registry_global_remove( void* data, struct wl_registry* registry, uint32_t name);

		CStack           * Input_Stack_;
		int previous_X = 960;
		int previous_Y = 540;
		struct xdg_toplevel_listener xdg_toplevel_listener = {
			.configure = xdg_toplevel_configure,
			.close     = xdg_toplevel_close,
			.configure_bounds = nullptr,
			.wm_capabilities   = nullptr
		};
		struct xdg_surface_listener xdg_surface_listener = {
			.configure = xdg_surface_configure
		};
		static struct wl_callback_listener callback_listener;
		static struct xdg_wm_base_listener shell_listener;
		static struct wl_keyboard_listener keyboard_listener;
		static struct wl_pointer_listener pointer_listener;
		static struct wl_seat_listener seat_lintener;
		struct wl_registry_listener registry_listener = {
			.global        = registry_global,
			.global_remove = registry_global_remove
		};
	};
}; ///< namespace GLVM::core

#endif
