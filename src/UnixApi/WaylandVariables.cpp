#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

struct wl_surface*    wl_surface;
struct wl_compositor* compositor;
struct xdg_toplevel*  xdg_topLevel;
struct xdg_wm_base*   xdg_shell;
struct wl_buffer*     buffer;
struct wl_shm*        shared_memory;
struct wl_seat*       seat;
struct wl_keyboard*   keyboard;
void* pixels;
uint16_t width = 480;
uint16_t height = 320;
uint8_t  constant_byte = 0;
uint8_t  close_xdg_toplevel;
struct wl_display*  display;
struct wl_registry* registry;
struct wl_callback* frame_callback;
struct xdg_surface* xdg_surface;
