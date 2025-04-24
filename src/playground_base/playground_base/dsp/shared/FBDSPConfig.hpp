#pragma once

#include <xsimd/xsimd.hpp>

// better make sure this all lines up
typedef xsimd::sse2 FBXSIMDBatchType;
template <class T>
using FBSIMDVector = xsimd::batch<T, FBXSIMDBatchType>;
template <class T>
using FBSIMDVectorSize = FBSIMDVector<T>::size;

inline int constexpr FBSIMDFloatCount = 4;
inline int constexpr FBSIMDDoubleCount = 2;
inline int constexpr FBSIMDAlign = FBSIMDFloatCount * sizeof(float);

inline int constexpr FBMaxVoices = 64;
inline int constexpr FBFixedBlockSamples = 4 * FBSIMDFloatCount;