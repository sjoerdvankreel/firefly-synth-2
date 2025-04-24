#pragma once

#include <xsimd/xsimd.hpp>

// better make sure this all lines up
typedef xsimd::sse2 FBXSIMDBatchType;
typedef xsimd::batch<float, FBXSIMDBatchType> FBXSIMDFloatBatch;

inline int constexpr FBMaxVoices = 64;
inline int constexpr FBSIMDFloatCount = 4;
inline int constexpr FBFixedBlockSamples = 4 * FBSIMDFloatCount;
inline int constexpr FBSIMDAlign = FBSIMDFloatCount * sizeof(float);

inline FBXSIMDFloatBatch
FBLoadSIMDAligned(float const* p)
{ return FBXSIMDFloatBatch::load_aligned(p); }

inline FBXSIMDFloatBatch
FBStoreSIMDAligned(FBXSIMDFloatBatch const& b, float* p)
{ b.store_aligned(p); }