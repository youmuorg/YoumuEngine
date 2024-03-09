#pragma once

// https://learn.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#undef CreateWindow

#include <cstdint>