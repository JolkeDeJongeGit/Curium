#pragma once
#include <winerror.h>
#include <exception>

inline void ThrowIfFailed(const HRESULT inHr)
{
    if (FAILED(inHr))
    {
        throw std::exception();
    }
}

