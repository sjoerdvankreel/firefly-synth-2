#include <firefly_synth/modules/mix/FFMixTopo.hpp>

std::string
FFMixFormatFXToFXSlot(FBStaticTopo const&, int /* moduleSlot */, int mixSlot)
{
  switch (mixSlot)
  {
  case 0: return "FX 1\U000021922";
  case 1: return "FX 1\U000021923";
  case 2: return "FX 2\U000021923";
  case 3: return "FX 1\U000021924";
  case 4: return "FX 2\U000021924";
  case 5: return "FX 3\U000021924";
  default: FB_ASSERT(false); return "";
  }
}

std::string
FFMixFormatFXToOutSlot(FBStaticTopo const&, int /* moduleSlot */, int mixSlot)
{
  return "FX " + std::to_string(mixSlot + 1) + "\U00002192Out";
}