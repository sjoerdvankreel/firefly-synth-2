#pragma once

#include <xsimd/xsimd.hpp>

#define FB_ARCH_TYPE_WIN_LIN_SSE2 1
#define FB_ARCH_TYPE_WIN_LIN_AVX2 2
#define FB_ARCH_TYPE_MAC_UNIVERSAL 3

#ifndef FB_ARCH_TYPE
#error
#endif

#if FB_ARCH_TYPE == FB_ARCH_TYPE_WIN_LIN_SSE2
typedef xsimd::batch<float, xsimd::sse2> FBFloatVector;
#elif FB_ARCH_TYPE == FB_ARCH_TYPE_WIN_LIN_AVX2
typedef xsimd::batch<float, xsimd::avx2> FBFloatVector;
#elif FB_ARCH_TYPE == FB_ARCH_TYPE_MAC_UNIVERSAL
#ifdef __x86_64__
typedef xsimd::batch<float, xsimd::sse2> FBFloatVector;
#elif defined(__aarch64__)
typedef xsimd::batch<float, xsimd::neon64> FBFloatVector;
#else
#error
#endif
#endif

inline int constexpr FBVectorFloatCount = sizeof(FBFloatVector) / sizeof(float);
