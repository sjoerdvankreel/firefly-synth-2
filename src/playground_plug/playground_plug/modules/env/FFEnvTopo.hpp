#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

enum class FFEnvParam { 
  On, Type, Sustain, Sync, Exp, Smooth,
  DelayTime, AttackTime, AttackSlope, HoldTime, 
  DecayTime, DecaySlope, ReleaseTime, ReleaseSlope, Count };
enum class FFEnvType { Sustain, Follow, Release };