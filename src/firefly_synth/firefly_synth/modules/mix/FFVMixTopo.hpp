#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFMixTopo.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeVMixTopo();

enum class FFAmpEnvTarget { Off, Osc, OscMix, FX, Out };
inline int constexpr FFVMixOsciToVFXCount = FFOsciCount * FFEffectCount;

inline int
FFVMixOsciToVFXGetOsciSlot(int mixSlot)
{ return mixSlot / FFEffectCount; }
inline int
FFVMixOsciToVFXGetFXSlot(int mixSlot)
{ return mixSlot % FFEffectCount; }

enum class FFVMixParam {
  Amp, AmpEnvToAmp, AmpEnvTarget,
  Bal, LFO6ToBal,
  OsciToOsciMix, OsciMixToVFX,
  OsciToVFX, VFXToVFX,
  OsciToOut, VFXToOut, OsciMixToOut, Count };