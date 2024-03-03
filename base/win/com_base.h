#pragma once

#include "win32_base.h"

namespace base {
namespace win {

class ComApartmentScope {
public:
  ComApartmentScope() {
    ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  }

  ~ComApartmentScope() {
    ::CoUninitialize();
  }
};

}
}