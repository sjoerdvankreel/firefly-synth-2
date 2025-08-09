#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

inline int constexpr FFEnvStageCount = 12;

enum class FFEnvCVOutput { Output, Count };
enum class FFEnvType { Off, Linear, Exp };
enum class FFEnvParam {
  Type, Sync, Release, SmoothTime, SmoothBars, LoopStart, LoopLength,
  StageLevel, StageSlope, StageTime, StageBars, Count };