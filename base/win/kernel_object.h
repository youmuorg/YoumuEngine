#pragma once

#include "win32_common.h"

namespace base {
namespace win {

class KernelObject {
public:
  KernelObject() : _handle(NULL) {}
  KernelObject(HANDLE handle) : _handle(handle) {}
  ~KernelObject() { Close(); }

  bool IsNull() const { return _handle == NULL; }
  bool IsInvalid() const { return _handle == INVALID_HANDLE_VALUE; }
  bool IsValid() const { return !IsNull() && !IsInvalid(); }

  void Reset(HANDLE handle) {
    Close();
    _handle = handle;
  }

  void Close() {
    if (IsValid()) {
      ::CloseHandle(_handle);
      _handle = NULL;
    }
  }

  HANDLE handle() const { return _handle; }

protected:
  HANDLE _handle;
};

}  // namespace win
}  // namespace base
