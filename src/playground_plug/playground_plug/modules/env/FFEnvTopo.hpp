#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

inline int constexpr FFEnvStageCount = 8;

enum class FFEnvMode { Linear, Exp };
//enum class FFEnvType { Sustain, Follow, Release };
/*
enum class FFEnvParam { 
  On, Type, Sync, Mode, 
  DelayTime, AttackTime, HoldTime, DecayTime, ReleaseTime, SmoothTime,
  DelayBars, AttackBars, HoldBars, DecayBars, ReleaseBars, SmoothBars,
  AttackSlope, DecaySlope, ReleaseSlope, SustainLevel, Count };
  */
enum class FFEnvParam {
  On, Sync, Mode, SmoothTime, LoopStart, LoopLength, Sustain, Release,
  SmoothBars, StageLevel, StageSlope, StageTime, StageBars, Count };