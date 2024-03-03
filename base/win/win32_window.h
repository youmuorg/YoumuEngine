#pragma once

#include "win32_base.h"

namespace base {
namespace win {

LRESULT CALLBACK _WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

struct WindowProperties{
  DWORD exStyle = 0;
  DWORD style = WS_OVERLAPPEDWINDOW;
  int x = CW_USEDEFAULT;
  int y = CW_USEDEFAULT;
  int width = CW_USEDEFAULT;
  int height = CW_USEDEFAULT;
  HMENU hmenu = NULL;
  HINSTANCE hinst = NULL;
};

constexpr WindowProperties overlappedWindowProperties;

class Win32Window {
public:
  Win32Window(const WindowProperties& props);

  void SetTitle(const wchar_t* title);
  void Close();
  void Show();

  HWND handle() const { return _hwnd; }
  int clientWidth() const { return _clientWidth; }
  int clientHeight() const { return _clientHeight; }

protected:
  virtual bool OnPaint(WPARAM wparam, LPARAM lparam, LRESULT& lresult) { return false; }
  virtual bool OnCreate(WPARAM wparam, LPARAM lparam, LRESULT& lresult) { return false; }
  virtual bool OnDestroy(WPARAM wparam, LPARAM lparam, LRESULT& lresult) { return false; }
  virtual void OnResize(UINT width, UINT height) {}
  virtual void OnMove(INT x, INT y) {}

private:
  bool ProcessWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& lresult);

private:
  HWND _hwnd = NULL;
  int _clientWidth = 0;
  int _clientHeight = 0;
  bool _quitAfterClosed = true;

  friend LRESULT _WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

} // namespace win
} // namespace base
