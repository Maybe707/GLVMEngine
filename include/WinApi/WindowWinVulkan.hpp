#pragma once
#include "IWindow.hpp"
#include "EventsStack.hpp"
#include <windows.h>
#include <cstdint>

namespace GLVM::core {
class WindowWinVulkan final : public IWindow {
public:
    uint16_t width = 1280, height = 720;
    WindowWinVulkan();
    ~WindowWinVulkan() override { Close(); }
    void configureWindow();
    void SwapBuffers() override {}
    void ClearDisplay() override {}
    bool HandleEvent(CEvent& event) override;
    HWND GetModernWindowHWND() const noexcept { return window_; }
    void Close() override;
    void CursorLock(int x, int y, int* offsetX, int* offsetY) override;
    static LRESULT CALLBACK MainWndProc(HWND window, UINT message, WPARAM key, LPARAM data);
private:
    HWND window_ = nullptr;
    CEvent* event_ = nullptr;
    bool minimized_ = false;
    bool cursorCaptured_ = false;
    void emit(EEvents event);
};
}
