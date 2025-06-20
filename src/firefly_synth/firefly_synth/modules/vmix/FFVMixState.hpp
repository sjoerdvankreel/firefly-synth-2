#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/vmix/FFVMixTopo.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFVMixDSPState final
{
  friend class FFVoiceProcessor;
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVMixDSPState);
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFVMixAccParamState final
{
  friend class FFVoiceProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeVMixTopo();
  std::array<TAccurate, FFOsciCount> osciToOut = {};
  std::array<TAccurate, FFEffectCount> VFXToOut = {};
  std::array<TAccurate, FFVMixVFXToVFXCount> VFXToVFX = {};
  std::array<TAccurate, FFVMixOsciToVFXCount> osciToVFX = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVMixAccParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFVMixParamState final
{
  friend class FFVoiceProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeVMixTopo();
  FFVMixAccParamState<TAccurate> acc = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVMixParamState);
};