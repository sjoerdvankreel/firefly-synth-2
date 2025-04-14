#pragma once

inline int constexpr FBMaxVoices = 64;
inline int constexpr FBSIMDFloatCount = 4;
inline int constexpr FBFixedBlockSamples = 16;
inline int constexpr FBFixedBlockAlign = FBSIMDFloatCount * sizeof(float);