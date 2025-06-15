#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeVMixTopo();
inline int constexpr FFVMixOsciToVFXCount = FFOsciCount * FFEffectCount;
inline int constexpr FFVMixStringOsciToVFXCount = FFStringOsciCount * FFEffectCount;
inline int constexpr FFVMixVFXToVFXCount = FFEffectCount * (FFEffectCount - 1) / 2;

inline int
FFVMixOsciToVFXGetOsciSlot(int mixSlot)
{ return mixSlot % FFOsciCount; }
inline int
FFVMixOsciToVFXGetFXSlot(int mixSlot)
{ return mixSlot / FFEffectCount; }
inline int
FFVMixStringOsciToVFXGetStringOsciSlot(int mixSlot)
{ return mixSlot % FFStringOsciCount; }
inline int
FFVMixStringOsciToVFXGetFXSlot(int mixSlot)
{ return mixSlot / FFEffectCount; }

enum class FFVMixParam {
  OsciToVFX, StringOsciToVFX, /*VFXToVFX,*/
  OsciToOut, /*StringOsciToOut,*/ VFXToOut, Count };