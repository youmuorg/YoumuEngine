#include "win32_window.h"
#include "error.h"

#include <mutex>

namespace base {
namespace win {

constexpr wchar_t _kBaseOverlappedWindowClassName[] =
    L"BaseOverlappedWindowClass";
constexpr wchar_t _kBaseOverlappedWindowName[] = L"BaseOverlappedWindow";
constexpr wchar_t _kWindowClassProp[] = L"WindowClass";

static LRESULT CALLBACK _WindowProc(HWND hwnd,
                                         UINT msg,
                                         WPARAM wparam,
                                         LPARAM lparam) {
  Win32Window* wnd = nullptr;
  if (msg == WM_NCCREATE) {
    // 非客户区创建时，把窗口关联到一个窗口实例。
    CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lparam);
    if (cs != nullptr) {
      wnd = reinterpret_cast<Win32Window*>(cs->lpCreateParams);
      if (wnd != nullptr) {
        BOOL bret =
            ::SetPropW(hwnd, _kWindowClassProp, reinterpret_cast<HANDLE>(wnd));
        _ApiCheckIfNot("SetPropW", bret != FALSE);
      }
    }
  } else if (msg == WM_NCDESTROY) {
    HANDLE handle = ::RemovePropW(hwnd, _kWindowClassProp);
    //_ApiCheckIf("RemovePropW", handle != NULL);
    wnd = reinterpret_cast<Win32Window*>(handle);
  } else {
    HANDLE handle = ::GetPropW(hwnd, _kWindowClassProp);
    //_ApiCheckIf("GetPropW", handle != NULL);
    wnd = reinterpret_cast<Win32Window*>(handle);
  }

  if (wnd != nullptr) {
    LRESULT lresult = 0;
    bool isHandle =
        wnd->ProcessWindowMessage(hwnd, msg, wparam, lparam, lresult);
    if (isHandle) {
      return lresult;
    }
  }
  return ::DefWindowProcW(hwnd, msg, wparam, lparam);
}

Win32Window::Win32Window() {}

ATOM Win32Window::RegisterWindowClass() {
  HINSTANCE hinst = ::GetModuleHandleW(nullptr);

  WNDCLASSEXW wcex{0};
  wcex.cbSize = sizeof(wcex);

  // 大小改变时通知重绘。
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = _WindowProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hinst;
  wcex.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
  wcex.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 默认背景色
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = _kBaseOverlappedWindowClassName;
  wcex.hIconSm = nullptr;

  ATOM atom = ::RegisterClassExW(&wcex);
  _ApiCheckIfNot("RegisterClassExW", atom != 0);
  return atom;
}

HWND Win32Window::CreateOverlappedWindow(int x, int y, int width, int height) {
  HINSTANCE hinst = ::GetModuleHandleW(nullptr);

  ::CreateWindowExW(0, _kBaseOverlappedWindowClassName,
                    _kBaseOverlappedWindowName, WS_OVERLAPPEDWINDOW, x, y,
                    width, height, HWND_DESKTOP, NULL, hinst,
                    reinterpret_cast<void*>(this));
  _ApiCheckIfNot("CreateWindowExW", _hwnd != NULL);
  return _hwnd;
}

void Win32Window::Close() {
  _CheckIfNot(_hwnd != NULL);

  ::CloseWindow(_hwnd);
}

void Win32Window::Show() {
  _CheckIfNot(_hwnd != NULL);

  ::ShowWindow(_hwnd, SW_SHOW);
}

void Win32Window::SetTitle(const wchar_t* title) {
  _CheckIfNot(_hwnd != NULL);

  ::SetWindowTextW(_hwnd, title);
}

bool Win32Window::ProcessWindowMessage(HWND hwnd,
                                       UINT msg,
                                       WPARAM wparam,
                                       LPARAM lparam,
                                       LRESULT& lresult) {
  bool bhandle = false;

  switch (msg) {
    case WM_NCCREATE:
      // 非客户区创建
      _hwnd = hwnd;
      break;
    case WM_CREATE:
      // 客户区创建
      bhandle = this->OnCreate(wparam, lparam, lresult);
      break;
    case WM_SIZING:
      break;
    case WM_SIZE: {
      _clientWidth = LOWORD(lparam);
      _clientHeight = HIWORD(lparam);
      this->OnResize(_clientWidth, _clientHeight);
    }
      return 0;
    case WM_MOVING:
      break;
    case WM_MOVE: {
      _screenX = LOWORD(lparam);
      _screenY = HIWORD(lparam);
      this->OnMove(_screenX, _screenY);
    }
      return 0;
    case WM_PAINT:
      // 客户区更新请求
      bhandle = this->OnPaint(wparam, lparam, lresult);
      break;
    case WM_DESTROY:
      // 客户区销毁
      bhandle = this->OnDestroy(wparam, lparam, lresult);
      if (_quitAfterClosed) {
        ::PostQuitMessage(0);
      }
      break;
    case WM_NCDESTROY:
      // 非客户区销毁
      _hwnd = nullptr;
      break;
    default:
      break;
  }

  return bhandle;
}

}  // namespace win
}  // namespace base
