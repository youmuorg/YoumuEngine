#pragma once

#include <base/win/com_common.h>
#include <base/win/com_error.h>
#include <base/win/error.h>

#include <wrl.h>

namespace base {
namespace dx {

using Microsoft::WRL::ComPtr;
using win::_ThrowIfFailed;
using win::_ThrowIfError;
using win::_ThrowIfNot;
using win::_ThrowMessageIfNot;

}
}