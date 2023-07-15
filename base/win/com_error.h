#pragma once

#include "error.h"

#include <windows.h>
#include <string>
#include <stdexcept>
#include <exception>

namespace base {
namespace win {

class ComError : public std::exception {
public:
  ComError(const std::source_location& location) 
   : _sourceLocation(location) {}
  ComError(std::string errorMsg, const std::source_location& location) 
   : _errorMsg(errorMsg),
     _sourceLocation(location) {}

  virtual char const* what() const {
      return !_errorMsg.empty() ? _errorMsg.c_str() : "Unknown COM exception";
  }

  std::source_location& sourceLocation() { return _sourceLocation; }
  std::string errorMsg() { return _errorMsg; }

private:
  std::string _errorMsg;
  std::source_location _sourceLocation;
};

inline void _ComCheckIfError(
    HRESULT hr,
    const std::source_location location = 
        std::source_location::current()) {

}

inline void _ComThrowIfError(
    HRESULT hr, 
    const std::source_location location = 
        std::source_location::current()) {
  if (FAILED(hr)) {
    throw ComError(location);
  }
}

inline void _ComThrow(
    std::string errorMsg,
    const std::source_location location = 
        std::source_location::current()) {
  throw ComError(errorMsg, location);
}

} // namespace win
} // namespace base
