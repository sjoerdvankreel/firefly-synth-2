#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

inline int constexpr FFEnvStageCount = 8;
enum class FFEnvParam {
  On, Sync, Exp, Sustain, Release, SmoothTime, LoopStart, LoopLength,
  SmoothBars, StageLevel, StageSlope, StageTime, StageBars, Count };