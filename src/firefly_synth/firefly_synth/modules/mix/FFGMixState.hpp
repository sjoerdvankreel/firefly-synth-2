#pragma once

#include <firefly_synth/modules/mix/FFGMixTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

template <class TAccurate>
class alignas(alignof(TAccurate)) FFGMixAccParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGMixTopo(bool);
  std::array<TAccurate, 1> amp = {};
  std::array<TAccurate, 1> bal = {};
  std::array<TAccurate, 1> lfo5ToAmp = {};
  std::array<TAccurate, 1> lfo6ToBal = {};
  std::array<TAccurate, 1> voiceToOut = {};
  std::array<TAccurate, 1> audioInToOut = {};
  std::array<TAccurate, 1> sidechainToOut = {};
  std::array<TAccurate, FFEffectCount> GFXToOut = {};
  std::array<TAccurate, FFEffectCount> voiceToGFX = {};
  std::array<TAccurate, FFEffectCount> audioInToGFX = {};
  std::array<TAccurate, FFEffectCount> sidechainToGFX = {};
  std::array<TAccurate, FFMixFXToFXCount> GFXToGFX = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGMixAccParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFGMixParamState final
{
  friend class FFPlugProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGMixTopo(bool);
  FFGMixAccParamState<TAccurate> acc = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGMixParamState);
};