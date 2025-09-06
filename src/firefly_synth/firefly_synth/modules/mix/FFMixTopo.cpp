#include <firefly_synth/modules/mix/FFMixTopo.hpp>

std::string
FFMixFormatFXToFXSlot(FBStaticTopo const&, bool global, int /* moduleSlot */, int mixSlot)
{
  std::string prefix = global ? "G" : "V";
  switch (mixSlot)
  {
  case 0: return prefix + "FX 1\U000021922";
  case 1: return prefix + "FX 1\U000021923";
  case 2: return prefix + "FX 2\U000021923";
  case 3: return prefix + "FX 1\U000021924";
  case 4: return prefix + "FX 2\U000021924";
  case 5: return prefix + "FX 3\U000021924";
  default: FB_ASSERT(false); return "";
  }
}

std::string
FFMixFormatFXToOutSlot(FBStaticTopo const&, bool global, int /* moduleSlot */, int mixSlot)
{
  std::string prefix = global ? "G" : "V";
  return prefix + "FX " + std::to_string(mixSlot + 1) + "\U00002192Out";
}