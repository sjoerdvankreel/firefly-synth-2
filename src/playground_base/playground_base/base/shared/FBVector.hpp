#pragma once

#include <xsimd/xsimd.hpp>

#define FB_ARCH_TYPE_WIN_LIN_SSE2 1
#define FB_ARCH_TYPE_MAC_UNIVERSAL 2

#ifndef FB_ARCH_TYPE
#error
#endif

#if FB_ARCH_TYPE == FB_ARCH_TYPE_WIN_LIN_SSE2
typedef xsimd::batch<float, xsimd::sse2> FBFloatVector;
typedef xsimd::batch<double, xsimd::sse2> FBDoubleVector;
#elif FB_ARCH_TYPE == FB_ARCH_TYPE_MAC_UNIVERSAL
#ifdef __x86_64__
typedef xsimd::batch<float, xsimd::sse2> FBFloatVector;
typedef xsimd::batch<double, xsimd::sse2> FBDoubleVector;
#elif defined(__aarch64__)
typedef xsimd::batch<float, xsimd::neon64> FBFloatVector;
typedef xsimd::batch<double, xsimd::neon64> FBDoubleVector;
#else
#error
#endif
#endif

inline int constexpr FBVectorFloatCount = sizeof(FBFloatVector) / sizeof(float);
inline int constexpr FBVectorDoubleCount = sizeof(FBDoubleVector) / sizeof(double);
