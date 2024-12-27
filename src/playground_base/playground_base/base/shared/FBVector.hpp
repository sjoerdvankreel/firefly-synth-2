#pragma once

#include <xsimd/xsimd.hpp>

// todo neon
#if FB_USE_SSE
typedef xsimd::batch<float, xsimd::sse2> FBFloatVector;
#elif FB_USE_AVX
typedef xsimd::batch<float, xsimd::avx2> FBFloatVector;
#else
#error
#endif

inline int constexpr FBVectorFloatCount = sizeof(FBFloatVector) / sizeof(float);