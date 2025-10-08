#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFMixTopo.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFVMixOsciToVFXCount = FFOsciCount * FFEffectCount;

enum class FFVMixAmpEnvTarget { 
  Off, 
  OscPreMix, OscPostMix, OscMix, 
  VFXIn, VFXOut, MixIn, MixOut };

std::unique_ptr<FBStaticModule> FFMakeVMixTopo();

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