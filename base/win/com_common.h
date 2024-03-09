#pragma once

#include "win32_common.h"

#include <objbase.h>

namespace base {
namespace win {

// https://learn.microsoft.com/en-us/windows/win32/com/processes--threads--and-apartments
class ComApartment {
public:
  static void Initialize();
};

}
}