#pragma once

#include "win32_common.h"

namespace base {
namespace win {

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
  bool ProcessWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& lresult);

  HWND handle() const { return _hwnd; }
  int32_t screenX() const { return _screenX; }
  int32_t screenY() const { return _screenY; }
  uint32_t clientWidth() const { return _clientWidth; }
  uint32_t clientHeight() const { return _clientHeight; }

protected:
  virtual bool OnPaint(WPARAM wparam, LPARAM lparam, LRESULT& lresult) { return false; }
  virtual bool OnCreate(WPARAM wparam, LPARAM lparam, LRESULT& lresult) { return false; }
  virtual bool OnDestroy(WPARAM wparam, LPARAM lparam, LRESULT& lresult) { return false; }
  virtual void OnResize(uint32_t width, uint32_t height) {}
  virtual void OnMove(int32_t x, int32_t y) {}

private:
  HWND _hwnd = NULL;
  bool _quitAfterClosed = true;
  int32_t _screenX = 0;
  int32_t _screenY = 0;
  uint32_t _clientWidth = 0;
  uint32_t _clientHeight = 0;
};

} // namespace win
} // namespace base
