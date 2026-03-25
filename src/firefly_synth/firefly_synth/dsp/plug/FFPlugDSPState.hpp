#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>

class alignas(FBSIMDAlign) FFPlugDSPState final
{
  friend class FFPlugProcessor;
  friend class FFEffectProcessor;
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFPlugDSPState);
  FBSArray2<float, FBFixedBlockSamples, 2> voiceMixdown = {};
};