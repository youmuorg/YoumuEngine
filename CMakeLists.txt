# ===
# CMake settings
# ===
cmake_minimum_required(VERSION 3.25)

# ===
# Global project settings
# ===
set(ENGINE_NAME "dream_engine")
project(
  ${ENGINE_NAME}
  VERSION 0.0.1
)
message("!!! Using CMake version ${CMAKE_VERSION}")
message("!!! Using Host System ${CMAKE_HOST_SYSTEM_NAME} version ${CMAKE_HOST_SYSTEM_VERSION}")
message("!!! Using Target System ${CMAKE_SYSTEM_NAME} version ${CMAKE_SYSTEM_VERSION}")
message("!!! Using Generator ${CMAKE_GENERATOR} platform ${CMAKE_GENERATOR_PLATFORM} path ${CMAKE_GENERATOR_INSTANCE}")
message("!!! Using Project ${CMAKE_PROJECT_NAME} version ${CMAKE_PROJECT_VERSION}")
message("!!! Using Source Directory ${CMAKE_SOURCE_DIR}")
message("!!! Using Binary Directory ${CMAKE_BINARY_DIR}")


# ===
# Global compiler settings
# ===
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options("/source-charset:utf-8")
  add_compile_options("/MP")
endif ()
message("!!! Using C++ Compiler ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION} path ${CMAKE_CXX_COMPILER}")
message("!!! Using C++ Standard C++ ${CMAKE_CXX_STANDARD}")
message("!!! Using C++ FLAGS ${CMAKE_CXX_FLAGS}")
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  message("!!! Using Windows SDK ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
  add_compile_definitions("_UNICODE" "UNICODE")
endif()

# ===
# Sub project settings
# ===
add_subdirectory("base")
add_subdirectory("editor")
