#pragma once

#include <xsimd/xsimd.hpp>

// better make sure this all lines up
#if FB_APPLE_AARCH64
typedef xsimd::neon64 FBXSIMDBatchType;
#else
typedef xsimd::sse2 FBXSIMDBatchType;
#endif

static_assert(xsimd::batch<float, FBXSIMDBatchType>::size == 4);
static_assert(xsimd::batch<double, FBXSIMDBatchType>::size == 2);

inline int constexpr FBSIMDFloatCount = 4;
inline int constexpr FBSIMDDoubleCount = 2;
inline int constexpr FBSIMDAlign = FBSIMDFloatCount * sizeof(float);
inline int constexpr FBFixedBlockSamples = 4 * FBSIMDFloatCount;

template <class T> using FBBatch = xsimd::batch<T, FBXSIMDBatchType>;
template <class T> using FBBoolBatch = xsimd::batch_bool<T, FBXSIMDBatchType>;
template <class T> struct FBSIMDTraits {
  static inline int constexpr Size = FBBatch<T>::size;
  static inline int constexpr Align = Size * sizeof(T); };
