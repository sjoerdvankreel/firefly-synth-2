#pragma once

#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

// Bit different from the rest as the series data
// is non-existent and the indicators are what matters.
struct alignas(FBSIMDAlign) FFGlobalUniExchangeState: 
public FBModuleProcSingleExchangeState
{
  ~FFGlobalUniExchangeState() = default;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalUniExchangeState);
  bool ShouldGraph(int /*graphIndex*/) const override { return boolIsActive != 0; }
};

class alignas(FBSIMDAlign) FFGlobalUniDSPState final
{
public:
  // public because everyone needs global unison
  std::unique_ptr<FFGlobalUniProcessor> processor = {};
  FBSArray<float, FBFixedBlockSamples> fakeGraphOutput = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGlobalUniDSPState);
  FFGlobalUniDSPState() : processor(std::make_unique<FFGlobalUniProcessor>()) {}
};

template <class TBlock>
class alignas(alignof(TBlock)) FFGlobalUniBlockParamState final
{
  friend class FFPlugProcessor;
  friend class FFGlobalUniProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
  std::array<TBlock, 1> voiceCount = {};
  std::array<TBlock, (int)FFGlobalUniTarget::Count> mode = {};
  std::array<TBlock, (int)FFGlobalUniTarget::Count> opType = {};
  std::array<TBlock, (int)FFGlobalUniTarget::Count> autoRandSeed = {};
  std::array<TBlock, (int)FFGlobalUniTarget::Count> autoRandFree = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalUniBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFGlobalUniAccParamState final
{
  friend class FFGlobalUniProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
  std::array<TAccurate, (int)FFGlobalUniTarget::Count> autoRand = {};
  std::array<TAccurate, (int)FFGlobalUniTarget::Count> autoSkew = {};
  std::array<TAccurate, (int)FFGlobalUniTarget::Count> autoSpread = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualVoiceCoarse = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualVoiceFine = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualVMixAmp = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualVMixBal = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualOscPhaseOffset = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualOscPan = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualOscGain = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualOscFine = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualOscCoarse = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualLFOPhaseOffset = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualLFORate = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualLFOMin = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualLFOMax = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualLFOSkewAX = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualLFOSkewAY = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualVFXParamA = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualVFXParamB = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualVFXParamC = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualVFXParamD = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEnvSlope = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEnvStretch = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoFade = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoExtend = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoTapMix = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoTapDelay = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoTapBal = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoTapLPF = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoTapHPF = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoFdbkDelay = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoFdbkMix = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoFdbkAmt = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoFdbkLPF = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoFdbkHPF = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoReverbMix = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoReverbSize = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoReverbDamp = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoReverbLPF = {};
  std::array<TAccurate, FFGlobalUniMaxCount> manualEchoReverbHPF = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalUniAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFGlobalUniParamState final
{
  friend class FFPlugProcessor;
  friend class FFGlobalUniProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
  FFGlobalUniAccParamState<TAccurate> acc = {};
  FFGlobalUniBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalUniParamState);
};