#pragma once

#define FF_RESTRICT __restrict

#ifdef _MSC_VER
#define FF_EXPORT __declspec(dllexport)
#else
#error
#endif