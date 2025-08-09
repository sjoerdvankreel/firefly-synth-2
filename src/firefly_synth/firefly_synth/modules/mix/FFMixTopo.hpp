#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <string>

struct FBStaticTopo;
inline int constexpr FFMixFXToFXCount = FFEffectCount * (FFEffectCount - 1) / 2;

std::string
FFMixFormatFXToFXSlot(FBStaticTopo const&, int moduleSlot, int mixSlot);
std::string
FFMixFormatFXToOutSlot(FBStaticTopo const& topo, int moduleSlot, int mixSlot);

inline int
FFMixFXToFXGetSourceSlot(int mixSlot)
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
FFMixFXToFXGetTargetSlot(int mixSlot)
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
