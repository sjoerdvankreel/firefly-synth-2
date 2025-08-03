#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

template <bool Global>
struct EffectGraphRenderData;

class alignas(FBSIMDAlign) FFEffectDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend struct EffectGraphRenderData<true>;
  friend struct EffectGraphRenderData<false>;
  std::unique_ptr<FFEffectProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEffectDSPState);
  FFEffectDSPState() : processor(std::make_unique<FFEffectProcessor>()) {}
  FBSArray2<float, FBFixedBlockSamples, 2> input = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFEffectBlockParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo(bool);
  std::array<TBlock, 1> on = {};
  std::array<TBlock, 1> oversample = {};
  std::array<TBlock, 1> lastKeySmoothTime = {};
  std::array<TBlock, FFEffectBlockCount> kind = {};
  std::array<TBlock, FFEffectBlockCount> clipMode = {};
  std::array<TBlock, FFEffectBlockCount> foldMode = {};
  std::array<TBlock, FFEffectBlockCount> skewMode = {};
  std::array<TBlock, FFEffectBlockCount> stVarMode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFEffectAccParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo(bool);
  std::array<TAccurate, 1> trackingKey = {};
  std::array<TAccurate, FFEffectBlockCount> envAmt = {};
  std::array<TAccurate, FFEffectBlockCount> lfoAmt = {};
  std::array<TAccurate, FFEffectBlockCount> distMix = {};
  std::array<TAccurate, FFEffectBlockCount> distAmt = {};
  std::array<TAccurate, FFEffectBlockCount> distBias = {};
  std::array<TAccurate, FFEffectBlockCount> distDrive = {};
  std::array<TAccurate, FFEffectBlockCount> stVarRes = {};
  std::array<TAccurate, FFEffectBlockCount> stVarFreq = {};
  std::array<TAccurate, FFEffectBlockCount> stVarGain = {};
  std::array<TAccurate, FFEffectBlockCount> stVarKeyTrk = {};
  std::array<TAccurate, FFEffectBlockCount> combKeyTrk = {};
  std::array<TAccurate, FFEffectBlockCount> combResMin = {};
  std::array<TAccurate, FFEffectBlockCount> combResPlus = {};
  std::array<TAccurate, FFEffectBlockCount> combFreqMin = {};
  std::array<TAccurate, FFEffectBlockCount> combFreqPlus = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFEffectParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo(bool);
  FFEffectAccParamState<TAccurate> acc = {};
  FFEffectBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectParamState);
};