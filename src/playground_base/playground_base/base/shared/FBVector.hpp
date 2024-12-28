#pragma once

#include <xsimd/xsimd.hpp>

#define FB_ARCH_TYPE_SSE2 1
#define FB_ARCH_TYPE_AVX2 2
#define FB_ARCH_TYPE_AARCH64 3

#ifdef FB_ARCH_TYPE
#if FB_ARCH_TYPE == FB_ARCH_TYPE_SSE2
typedef xsimd::batch<float, xsimd::sse2> FBFloatVector;
#elif FB_ARCH_TYPE == FB_ARCH_TYPE_AVX2
#error no workie
typedef xsimd::batch<float, xsimd::avx2> FBFloatVector;
#elif FB_ARCH_TYPE == FB_ARCH_TYPE_AARCH64
typedef xsimd::batch<float, xsimd::neon64> FBFloatVector;
#endif
#else
#error
#endif

inline int constexpr FBVectorFloatCount = sizeof(FBFloatVector) / sizeof(float);