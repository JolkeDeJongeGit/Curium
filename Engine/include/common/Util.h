#pragma once
#include <winerror.h>
#include <exception>

inline void ThrowIfFailed(HRESULT hr);
