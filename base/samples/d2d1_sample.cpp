#include <base/win/win32_common.h>
#include <base/win/com_common.h>
#include <base/win/message_loop.h>
#include <base/dx/d2d1_window.h>
#include <base/dx/d2d1_layer.h>
#include <base/dx/d2d1_text_layer.h>

#include <iostream>
#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;
using namespace base;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  win::ComApartment::Initialize();
      
  dx::D2d1Window wnd;
  wnd.d2d1Compositor().AddLayer(std::make_unique<dx::D2d1QuadLayer>(wnd.d2d1RenderTarget()->renderTarget()));

  dx::DwriteFactory dwf;
  wnd.d2d1Compositor().AddLayer(std::make_unique<dx::D2d1DebugInfoLayer>(wnd.d2d1RenderTarget()->renderTarget(), dwf.factory()));

  wnd.Show();
  //wnd.Paint();
  //wnd.Capture();

  win::MessageLoop mq;
  mq.Run();

  return 0;
}