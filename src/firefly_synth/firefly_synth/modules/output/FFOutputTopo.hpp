#pragma once

#include <memory>

inline int constexpr FFMaxCpu = 99;
inline int constexpr FFCpuSamplingRate = 3;

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOutputTopo();
enum class FFOutputParam { Voices, Cpu, Count };