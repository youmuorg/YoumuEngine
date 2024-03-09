#pragma once

#include "kernel_object.h"
#include "error.h"

namespace base {
namespace win {

class Event : public KernelObject {
public:
  void Create() {
    Close();
    _handle = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
    _ApiThrowIfNot("CreateEventW", IsValid());
  }
};

}  // namespace win
}  // namespace base