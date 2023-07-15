#pragma once

#include <windows.h>

namespace base {
namespace win {

LRESULT CALLBACK _WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

struct OverlappedWindowParams{
  DWORD exStyle = 0;
  DWORD style = WS_OVERLAPPEDWINDOW;
  int x = CW_USEDEFAULT;
  int y = CW_USEDEFAULT;
  int width = CW_USEDEFAULT;
  int height = CW_USEDEFAULT;
  HMENU hmenu = NULL;
  HINSTANCE hinst = NULL;
};

constexpr OverlappedWindowParams overlappedWindow;

class Win32Window {
public:
  Win32Window(const OverlappedWindowParams& params);

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
  virtual bool OnResize(WPARAM wparam, LPARAM lparam, LRESULT& lresult) { return false; }

private:
  bool ProcessWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& lresult);

private:
  HWND _hwnd = NULL;
  int _clientWidth = 0;
  int _clientHeight = 0;

  friend LRESULT _WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

} // namespace win
} // namespace base
