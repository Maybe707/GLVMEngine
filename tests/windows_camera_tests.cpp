#include "WinApi/WindowWinVulkan.hpp"
#include "Rendering/CameraOrbit.hpp"
#include <cassert>
#include <iostream>

int main() {
    using namespace GLVM::core;
    WindowWinVulkan window;
    CEvent event;
    const HWND handle = window.GetModernWindowHWND();
    SetForegroundWindow(handle);
    // Windows can deny foreground activation from a background build process.
    // Click this test's own client area, just as a user activates the demo.
    ShowWindow(handle, SW_SHOW);
    SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    window.HandleEvent(event);
    RECT client{};
    assert(GetClientRect(handle, &client));
    POINT activation{client.right / 2, client.bottom / 2};
    assert(ClientToScreen(handle, &activation));
    if (WindowFromPoint(activation) != handle)
        std::cerr << "Window probe: visible=" << IsWindowVisible(handle) << " handle=" << handle
                  << " hit=" << WindowFromPoint(activation) << " center=" << activation.x << ',' << activation.y
                  << " client=" << client.right << 'x' << client.bottom << '\n';
    assert(WindowFromPoint(activation) == handle);
    assert(SetCursorPos(activation.x, activation.y));
    INPUT clicks[2]{};
    clicks[0].type = clicks[1].type = INPUT_MOUSE;
    clicks[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    clicks[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    assert(SendInput(2, clicks, sizeof(INPUT)) == 2);
    for (int i = 0; i < 50 && GetForegroundWindow() != handle; ++i) {
        window.HandleEvent(event);
        Sleep(20);
    }
    window.HandleEvent(event);
    assert(GetForegroundWindow() == handle);
    int dx = 1234, dy = 890;
    window.CursorLock(0, 0, &dx, &dy);
    assert(dx == 0 && dy == 0); // First capture ignores the old cursor position.
    GLVM::ecs::components::beholder camera{.Position = {0.0f, 5.0f, 1.0f}};
    auto motion = [&](int x, int y) {
        POINT center{};
        assert(GetCursorPos(&center));
        assert(SetCursorPos(center.x + x, center.y + y));
        window.CursorLock(0, 0, &dx, &dy);
        assert(dx == x && dy == y);
        rotateCameraOrbit(camera, dx, dy);
    };
    for (int i = 0; i < 20; ++i) motion(0, -100);
    assert(camera.forward[1] > 0.999f);
    const float top = camera.forward[1];
    motion(0, 1);
    assert(camera.forward[1] < top);
    motion(0, 0);
    // Losing and restoring focus must not replay the previous mouse delta.
    SendMessageW(handle, WM_KILLFOCUS, 0, 0);
    POINT center{};
    assert(GetCursorPos(&center));
    assert(SetCursorPos(center.x + 100, center.y + 100));
    window.CursorLock(0, 0, &dx, &dy);
    assert(dx == 0 && dy == 0);
    window.Close();
    std::cout << "PASS Win32 relative mouse motion beyond 890 pixels, camera reversal, idle and focus recapture\n";
}
