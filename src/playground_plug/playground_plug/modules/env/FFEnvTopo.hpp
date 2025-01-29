#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

enum class FFEnvParam { 
  On, Exp, Type,
  DelayTime, AttackTime, AttackSlope, 
  HoldTime, DecayTime, DecaySlope, 
  SustainLevel, ReleaseTime, ReleaseSlope,
  Count };
enum class FFEnvType { Sustain, Follow, Release };
