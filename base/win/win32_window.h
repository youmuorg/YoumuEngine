#pragma once

#include "win32_common.h"

namespace base {
namespace win {

LRESULT CALLBACK _WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


class Win32Window {
public:
  Win32Window();

public:
  static ATOM RegisterWindowClass();

public:
  // 创建顶层重叠窗口
  HWND CreateOverlappedWindow(int x = CW_USEDEFAULT,
                              int y = CW_USEDEFAULT,
                              int width = CW_USEDEFAULT,
                              int height = CW_USEDEFAULT);
  void SetTitle(const wchar_t* title);
  void Close();
  void Show();

  HWND handle() const { return _hwnd; }
  int32_t screenX() const { return _screenX; }
  int32_t screenY() const { return _screenY; }
  uint32_t clientWidth() const { return _clientWidth; }
  uint32_t clientHeight() const { return _clientHeight; }


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
  bool _quitAfterClosed = true;
  int32_t _screenX = 0;
  int32_t _screenY = 0;
  uint32_t _clientWidth = 0;
  uint32_t _clientHeight = 0;

  friend LRESULT _WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

} // namespace win
} // namespace base
