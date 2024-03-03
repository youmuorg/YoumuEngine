#pragma once

#include "dx_base.h"
#include <dwrite.h>

namespace base {
namespace win {
namespace dx {

class DwriteFactory {
public:
  DwriteFactory();

public:
  IDWriteFactory* factory() const { return _factory.Get(); }

private:
  ComPtr<IDWriteFactory> _factory;
};

}
}
}