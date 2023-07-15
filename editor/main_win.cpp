#include <base/win/message_queue.h>
#include <base/win/dx/d3d11_utils.h>
#include <base/win/dx/dxgi1_utils.h>
#include <base/win/dx/d3d11_window.h>

#include <unifex/timed_single_thread_context.hpp>
#include <unifex/on_stream.hpp>
#include <unifex/scheduler_concepts.hpp>
#include <unifex/range_stream.hpp>
#include <unifex/for_each.hpp>
#include <unifex/stop_when.hpp>
#include <unifex/sync_wait.hpp>
#include <unifex/then.hpp>
#include <windows.h>

#include <iostream>
#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;
using namespace base;
namespace dx = base::win::dx;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  // unifex::timed_single_thread_context ctx;
  // auto timed = unifex::on_stream(unifex::current_scheduler, unifex::range_stream{0, 20}) 
  //   | unifex::for_each([](int value) {
  //       std::cout << "value: " << value << std::endl;
  //       std::this_thread::sleep_for(10ms);
  //     })
  //   | unifex::stop_when(unifex::schedule_after(150ms) | unifex::then([](){
  //       std::cout << "end" << std::endl;
  //     }));
  // unifex::sync_wait(unifex::on(ctx.get_scheduler(), std::move(timed)));


  try {
    dx::D3d11Window wnd;

    wnd.Show();
    //wnd.Paint();
    //wnd.Capture();

    win::MessageQueue mq;
    while(1) {
      while(mq.Dispatch()){}
      try {
        wnd.Capture();
        wnd.Render();
      } catch (base::win::ComError error) {
        std::cout 
          << "[" << error.sourceLocation().file_name()
          << "(" << error.sourceLocation().line() << "," << error.sourceLocation().column() << ")] "
          << error.what();
          }
    }
  } catch (win::ComError& error) {
    std::cout 
      << "[" << error.sourceLocation().file_name()
      << "(" << error.sourceLocation().line() << "," << error.sourceLocation().column() << ")] "
      << error.what();
  }

  return 0;
}