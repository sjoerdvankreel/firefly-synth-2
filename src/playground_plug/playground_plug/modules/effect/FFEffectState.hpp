#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>
#include <playground_plug/modules/effect/FFEffectProcessor.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFEffectDSPState final
{
  friend class FFVoiceProcessor;
  friend struct EffectGraphRenderData;
  std::unique_ptr<FFEffectProcessor> processor = {};
public:
  FFEffectDSPState();
  ~FFEffectDSPState();
  FBSArray2<float, FBFixedBlockSamples, 2> input = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEffectDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFEffectBlockParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> oversample = {};
  std::array<TVoiceBlock, FFEffectBlockCount> kind = {};
  std::array<TVoiceBlock, FFEffectBlockCount> clipMode = {};
  std::array<TVoiceBlock, FFEffectBlockCount> foldMode = {};
  std::array<TVoiceBlock, FFEffectBlockCount> skewMode = {};
  std::array<TVoiceBlock, FFEffectBlockCount> stVarMode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEffectAccParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
  std::array<TVoiceAcc, 1> trackingKey = {};
  std::array<TVoiceAcc, FFEffectBlockCount> distMix = {};
  std::array<TVoiceAcc, FFEffectBlockCount> distAmt = {};
  std::array<TVoiceAcc, FFEffectBlockCount> distBias = {};
  std::array<TVoiceAcc, FFEffectBlockCount> distDrive = {};
  std::array<TVoiceAcc, FFEffectBlockCount> stVarRes = {};
  std::array<TVoiceAcc, FFEffectBlockCount> stVarFreq = {};
  std::array<TVoiceAcc, FFEffectBlockCount> stVarGain = {};
  std::array<TVoiceAcc, FFEffectBlockCount> stVarKeyTrk = {};
  std::array<TVoiceAcc, FFEffectBlockCount> combKeyTrk = {};
  std::array<TVoiceAcc, FFEffectBlockCount> combResMin = {};
  std::array<TVoiceAcc, FFEffectBlockCount> combResPlus = {};
  std::array<TVoiceAcc, FFEffectBlockCount> combFreqMin = {};
  std::array<TVoiceAcc, FFEffectBlockCount> combFreqPlus = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEffectParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
  FFEffectAccParamState<TVoiceAcc> acc = {};
  FFEffectBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectParamState);
};