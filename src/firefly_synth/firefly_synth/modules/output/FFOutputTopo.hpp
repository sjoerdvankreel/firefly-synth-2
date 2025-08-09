#pragma once

#include <memory>

inline int constexpr FFOutputMaxCpu = 999;
inline int constexpr FFOutputMaxGain = 999;
inline int constexpr FFOutputSamplingRate = 3;

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOutputTopo();
enum class FFOutputParam { Voices, Cpu, Gain, Count };