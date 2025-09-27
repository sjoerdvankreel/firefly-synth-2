#pragma once

#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFGlobalUniDSPState final
{
public:
  // public because everyone needs global unison
  std::unique_ptr<FFGlobalUniProcessor> processor = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGlobalUniDSPState);
  FFGlobalUniDSPState() : processor(std::make_unique<FFGlobalUniProcessor>()) {}
};

template <class TBlock>
class alignas(alignof(TBlock)) FFGlobalUniBlockParamState final
{
  friend class FFPlugProcessor;
  friend class FFGlobalUniProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
  std::array<TBlock, 1> type = {};
  std::array<TBlock, 1> voiceCount = {};
  std::array<TBlock, FFGlobalUniMaxCount> fullEchoFade = {};
  std::array<TBlock, FFGlobalUniMaxCount> fullEchoExtend = {};
  std::array<TBlock, FFGlobalUniMaxCount> fullEnvOffset = {};
  std::array<TBlock, FFGlobalUniMaxCount> fullEnvStretch = {};
  std::array<TBlock, FFGlobalUniMaxCount> fullOscPhaseRand = {};
  std::array<TBlock, FFGlobalUniMaxCount> fullOscPhaseOffset = {};
  std::array<TBlock, FFGlobalUniMaxCount> fullLFOPhaseRand = {};
  std::array<TBlock, FFGlobalUniMaxCount> fullLFOPhaseOffset = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalUniBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFGlobalUniAccParamState final
{
  friend class FFGlobalUniProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGlobalUniTopo();
  std::array<TAccurate, FFGlobalUniMaxCount> fullVoiceCoarse = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullVoiceFine = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullVMixAmp = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullVMixBal = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullOscPan = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullOscGain = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullOscFine = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullOscCoarse = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullLFORate = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullLFOMin = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullLFOMax = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullLFOSkewAX = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullLFOSkewAY = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullVFXParamA = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullVFXParamB = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullVFXParamC = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullVFXParamD = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoTapLevel = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoTapDelay = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoTapBal = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoFdbkDelay = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoFdbkMix = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoFdbkAmt = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoReverbMix = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoReverbSize = {};
  std::array<TAccurate, FFGlobalUniMaxCount> fullEchoReverbDamp = {};
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