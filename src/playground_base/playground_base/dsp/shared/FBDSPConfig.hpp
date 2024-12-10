#pragma once

inline int constexpr FBMaxVoices = 64;
inline int constexpr FBFixedBlockSize = 16;
inline int constexpr FBFixedBlockAlign = FBFixedBlockSize * sizeof(float); // todo SIMD
inline float constexpr FBParamSmoothSec = 0.001f;