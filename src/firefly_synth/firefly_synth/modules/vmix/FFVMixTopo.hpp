#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeVMixTopo();
inline int constexpr FFVMixOsciToVFXCount = FFOsciCount * FFEffectCount;
inline int constexpr FFVMixVFXToVFXCount = FFEffectCount * (FFEffectCount - 1) / 2;

inline int
FFVMixOsciToVFXGetFXSlot(int mixSlot)
{ return mixSlot / FFOsciCount; }
inline int
FFVMixOsciToVFXGetOsciSlot(int mixSlot)
{ return mixSlot % FFOsciCount; }

inline int
FFVMixVFXToVFXGetSourceSlot(int mixSlot)
{
  switch (mixSlot)
  {
  case 0: return 0;
  case 1: return 0;
  case 2: return 1;
  case 3: return 0;
  case 4: return 1;
  case 5: return 2;
  default: FB_ASSERT(false); return -1;
  }
}

inline int
FFVMixVFXToVFXGetTargetSlot(int mixSlot)
{
  switch (mixSlot)
  {
  case 0: return 1;
  case 1: return 2;
  case 2: return 2;
  case 3: return 3;
  case 4: return 3;
  case 5: return 3;
  default: FB_ASSERT(false); return -1;
  }
}

enum class FFVMixParam {
  OsciToVFX, VFXToVFX,
  OsciToOut, VFXToOut, Count };