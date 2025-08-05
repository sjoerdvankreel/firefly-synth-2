#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFMixTopo.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeVMixTopo();
inline int constexpr FFVMixOsciToVFXCount = FFOsciCount * FFEffectCount;

inline int
FFVMixOsciToVFXGetOsciSlot(int mixSlot)
{ return mixSlot / FFEffectCount; }
inline int
FFVMixOsciToVFXGetFXSlot(int mixSlot)
{ return mixSlot % FFEffectCount; }

enum class FFVMixParam {
  Amp, AmpEnvToAmp, Bal, LFO1ToBal,
  OsciToVFX, VFXToVFX,
  OsciToOut, VFXToOut, Count };