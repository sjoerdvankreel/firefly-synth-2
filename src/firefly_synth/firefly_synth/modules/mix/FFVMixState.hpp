#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFVMixTopo.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

template <class TBlock>
class alignas(alignof(TBlock)) FFVMixBlockParamState final
{
  friend class FFVoiceProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeVMixTopo();
  std::array<TBlock, 1> ampEnvTarget = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVMixBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFVMixAccParamState final
{
  friend class FFVoiceProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeVMixTopo();
  std::array<TAccurate, 1> bal = {};
  std::array<TAccurate, 1> lfo6ToBal = {};
  std::array<TAccurate, 1> amp = {};
  std::array<TAccurate, 1> ampEnvToAmp = {};
  std::array<TAccurate, 1> osciMixToOut = {};
  std::array<TAccurate, FFOsciCount> osciToOut = {};
  std::array<TAccurate, FFOsciCount> osciToOsciMix = {};
  std::array<TAccurate, FFEffectCount> osciMixToVFX = {};
  std::array<TAccurate, FFEffectCount> VFXToOut = {};
  std::array<TAccurate, FFMixFXToFXCount> VFXToVFX = {};
  std::array<TAccurate, FFVMixOsciToVFXCount> osciToVFX = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVMixAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFVMixParamState final
{
  friend class FFVoiceProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeVMixTopo();
  FFVMixAccParamState<TAccurate> acc = {};
  FFVMixBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVMixParamState);
};