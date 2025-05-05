#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>

inline int constexpr FBMaxVoices = 64;
inline int constexpr FBFixedBlockSamples = 4 * FBSIMDFloatCount;