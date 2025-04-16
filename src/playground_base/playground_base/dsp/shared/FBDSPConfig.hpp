#pragma once

#include <xsimd/xsimd.hpp>

// better make sure this all lines up
typedef xsimd::sse2 FBXSIMDBatchType;
inline int constexpr FBMaxVoices = 64;
inline int constexpr FBSIMDFloatCount = 4;
inline int constexpr FBFixedBlockSamples = 4 * FBSIMDFloatCount;
inline int constexpr FBSIMDAlign = FBSIMDFloatCount * sizeof(float);