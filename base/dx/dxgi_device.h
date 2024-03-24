#pragma once

#include "dx_common.h"

#include <dxgi1_6.h>

namespace base {
namespace dx {

class DxgiDevice {
public:
  void CreateFactory1();
  void CreateFactory6();

  IDXGIFactory1* factory1() {return _factory1.Get(); }
  IDXGIFactory2* factory2() {return _factory2.Get(); }
  IDXGIFactory6* factory6() {return _factory6.Get(); }

public:
  static void EnableDebug();

private:
  ComPtr<IDXGIFactory1> _factory1;
  ComPtr<IDXGIFactory2> _factory2;
  ComPtr<IDXGIFactory6> _factory6;
};

}
}