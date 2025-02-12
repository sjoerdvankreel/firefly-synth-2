#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

enum class FFEnvMode { Linear, Exp };
enum class FFEnvType { Sustain, Follow, Release };

enum class FFEnvParam { 
  On, Type, SustainLevel, Sync, Mode, 
  DelayTime, AttackTime, HoldTime, DecayTime, ReleaseTime, SmoothTime,
  DelayBars, AttackBars, HoldBars, DecayBars, ReleaseBars, SmoothBars,
  AttackSlope, DecaySlope, ReleaseSlope, Count };