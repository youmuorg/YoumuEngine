#pragma once 

#include "win32_common.h"

#include <stdexcept>
#include <source_location>


namespace base {
namespace win {

inline void _ThrowIfNot(bool expr, const std::source_location location = 
    std::source_location::current()) {
  if (!expr) {
    throw std::runtime_error{"expr is not true."};
  }
}

inline void _ThrowMessageIfNot(bool expr, std::string message, const std::source_location location = 
    std::source_location::current()) {
  if (!expr) {
    throw std::runtime_error{message};
  }
}

inline void _CheckIfNot(bool expr, const std::source_location location = 
    std::source_location::current()) {
  
}
  
class ApiError {

};

inline void _ApiThrowIfNot(
    const char* apiName, 
    bool expr, 
    const std::source_location location = 
        std::source_location::current()) {
  if (!expr) {
    throw ApiError{};
  }
}

inline void _ApiCheckIfNot(
    const char* apiName, 
    bool expr, 
    const std::source_location location = 
        std::source_location::current()) {
  if (!expr) {
    throw ApiError{};
  }
}

} // namespace win
} // namespace base
