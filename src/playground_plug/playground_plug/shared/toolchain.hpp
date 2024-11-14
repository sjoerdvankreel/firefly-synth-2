#pragma once

#ifdef _MSC_VER
#define FF_EXPORT __declspec(dllexport)
#else
#error
#endif

#define FF_RESTRICT __restrict