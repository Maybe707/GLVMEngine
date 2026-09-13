#include "WinApi/WindowWinVulkan.hpp"
#include <windowsx.h>
#include <algorithm>
#include <stdexcept>

namespace GLVM::core {
namespace { constexpr wchar_t windowClass[] = L"GLVMEngineWindow"; }

WindowWinVulkan::WindowWinVulkan() {
    SetProcessDPIAware();
    const HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSW description{};
    description.lpfnWndProc = MainWndProc;
    description.hInstance = instance;
    description.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    description.lpszClassName = windowClass;
    if (!RegisterClassW(&description) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        throw std::runtime_error("Cannot register the GLVM window class");
    RECT rect{0, 0, width, height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    window_ = CreateWindowExW(0, windowClass, L"GLVMEngine - Vulkan", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
                             rect.bottom - rect.top, nullptr, nullptr, instance, this);
    if (!window_) throw std::runtime_error("Cannot create the GLVM window");
    ShowWindow(window_, SW_SHOWNORMAL);
    UpdateWindow(window_);
    POINT center{width / 2, height / 2};
    ClientToScreen(window_, &center);
    SetCursorPos(center.x, center.y);
}

void WindowWinVulkan::configureWindow() {
    RECT client{};
    if (GetClientRect(window_, &client) && !minimized_) {
        width = static_cast<uint16_t>(std::max<LONG>(1, client.right));
        height = static_cast<uint16_t>(std::max<LONG>(1, client.bottom));
    }
}

bool WindowWinVulkan::HandleEvent(CEvent& event) {
    event_ = &event;
    do {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                emit(eGAME_LOOP_KILL);
                return false;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        if (minimized_ && Input_Stack_.SearchElement(eGAME_LOOP_KILL) != eGAME_LOOP_KILL)
            WaitMessage();
        else break;
    } while (true);
    return true;
}

void WindowWinVulkan::emit(EEvents event) {
    if (event_) event_->SetEvent(event);
    switch (event) {
    case eKEYRELEASE_W: Input_Stack_.Remove(eMOVE_FORWARD); break;
    case eKEYRELEASE_S: Input_Stack_.Remove(eMOVE_BACKWARD); break;
    case eKEYRELEASE_A: Input_Stack_.Remove(eMOVE_LEFT); break;
    case eKEYRELEASE_D: Input_Stack_.Remove(eMOVE_RIGHT); break;
    case eKEYRELEASE_JUMP: Input_Stack_.Remove(eJUMP); break;
    case eMOUSE_LEFT_BUTTON_RELEASE: Input_Stack_.Remove(eMOUSE_LEFT_BUTTON); break;
    default: Input_Stack_.Push(event); break;
    }
}

void WindowWinVulkan::Close() {
    ClipCursor(nullptr);
    if (window_) {
        HWND closing = window_;
        window_ = nullptr;
        DestroyWindow(closing);
    }
}

void WindowWinVulkan::CursorLock(int, int, int* offsetX, int* offsetY) {
    *offsetX = 0;
    *offsetY = 0;
    if (!window_ || minimized_ || GetForegroundWindow() != window_) return;
    RECT client{};
    GetClientRect(window_, &client);
    POINT topLeft{client.left, client.top}, bottomRight{client.right, client.bottom};
    ClientToScreen(window_, &topLeft);
    ClientToScreen(window_, &bottomRight);
    RECT clip{topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
    ClipCursor(&clip);
    POINT cursor{};
    GetCursorPos(&cursor);
    const POINT center{(clip.left + clip.right) / 2, (clip.top + clip.bottom) / 2};
    if (cursorCaptured_) {
        *offsetX = cursor.x - center.x;
        *offsetY = cursor.y - center.y;
    }
    cursorCaptured_ = true;
    SetCursorPos(center.x, center.y);
    SetCursor(nullptr);
}

LRESULT CALLBACK WindowWinVulkan::MainWndProc(HWND window, UINT message, WPARAM key, LPARAM data) {
    auto* self = reinterpret_cast<WindowWinVulkan*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        self = static_cast<WindowWinVulkan*>(reinterpret_cast<CREATESTRUCTW*>(data)->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    if (!self) return DefWindowProcW(window, message, key, data);
    switch (message) {
    case WM_CLOSE: self->emit(eGAME_LOOP_KILL); return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    case WM_SIZE:
        self->minimized_ = key == SIZE_MINIMIZED;
        if (!self->minimized_) {
            self->width = std::max<WORD>(1, LOWORD(data));
            self->height = std::max<WORD>(1, HIWORD(data));
        }
        return 0;
    case WM_KILLFOCUS:
        self->cursorCaptured_ = false;
        ClipCursor(nullptr);
        for (auto event : {eMOVE_FORWARD, eMOVE_BACKWARD, eMOVE_LEFT, eMOVE_RIGHT, eJUMP, eMOUSE_LEFT_BUTTON})
            Input_Stack_.Remove(event);
        return 0;
    case WM_MOUSEMOVE:
        if (self->event_) {
            self->event_->mousePointerPosition.position_X = GET_X_LPARAM(data);
            self->event_->mousePointerPosition.position_Y = GET_Y_LPARAM(data);
        }
        return 0;
    case WM_LBUTTONDOWN: self->emit(eMOUSE_LEFT_BUTTON); return 0;
    case WM_LBUTTONUP:
        if (self->event_) self->event_->isLeftMouseButtonReleased = true;
        self->emit(eMOUSE_LEFT_BUTTON_RELEASE);
        return 0;
    case WM_KEYDOWN:
        switch (key) {
        case VK_ESCAPE: self->emit(eGAME_LOOP_KILL); break;
        case 'W': self->emit(eMOVE_FORWARD); break;
        case 'S': self->emit(eMOVE_BACKWARD); break;
        case 'A': self->emit(eMOVE_LEFT); break;
        case 'D': self->emit(eMOVE_RIGHT); break;
        case VK_SPACE: self->emit(eJUMP); break;
        case 'I': if (!(data & (1LL << 30))) self->emit(eINVENTORY); break;
        case 'O': if (!(data & (1LL << 30))) self->emit(eDEBUG_COLLISIONS_ACTIVE); break;
        }
        return 0;
    case WM_KEYUP:
        switch (key) {
        case 'W': self->emit(eKEYRELEASE_W); break;
        case 'S': self->emit(eKEYRELEASE_S); break;
        case 'A': self->emit(eKEYRELEASE_A); break;
        case 'D': self->emit(eKEYRELEASE_D); break;
        case VK_SPACE: self->emit(eKEYRELEASE_JUMP); break;
        }
        return 0;
    }
    return DefWindowProcW(window, message, key, data);
}
}
