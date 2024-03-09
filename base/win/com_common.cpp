#include "com_common.h"
#include "com_error.h"

namespace base {
namespace win {

void ComApartment::Initialize() {
  HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  _ComThrowIfError(hr);
}
}  // namespace win
}  // namespace base