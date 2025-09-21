#pragma once

#include <firefly_synth/modules/master/FFMasterTopo.hpp>
#include <firefly_synth/modules/master/FFMasterProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

class MTSClient;
struct FBStaticModule;

class alignas(FBSIMDAlign) FFMasterDSPState final
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFMasterDSPState);
  FFMasterDSPState() : processor(std::make_unique<FFMasterProcessor>()) {}

  // public because everyone needs global unison
  std::unique_ptr<FFMasterProcessor> processor = {};

  bool mtsEspOn = {};
  MTSClient* mtsClient = {};
  FFMasterTuningMode tuningMode = {};
  FFMasterPitchBendTarget bendTarget = {};
  FBSArray<float, FBFixedBlockSamples> bendAmountInSemis = {};

  FBSArray<float, FBFixedBlockSamples> outputMod = {};
  FBSArray<float, FBFixedBlockSamples> outputBend = {};
  FBSArray2<float, FBFixedBlockSamples, FFMasterAuxCount> outputAux = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFMasterBlockParamState final
{
  friend class FFPlugProcessor;
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TBlock, 1> uniType = {};
  std::array<TBlock, 1> uniCount = {};
  std::array<TBlock, 1> tuningMode = {};
  std::array<TBlock, 1> hostSmoothTime = {};
  std::array<TBlock, 1> pitchBendRange = {};
  std::array<TBlock, 1> pitchBendTarget = {};
  std::array<TBlock, FFMasterUniMaxCount> uniFullEchoFade = {};
  std::array<TBlock, FFMasterUniMaxCount> uniFullEchoExtend = {};
  std::array<TBlock, FFMasterUniMaxCount> uniFullEnvOffset = {};
  std::array<TBlock, FFMasterUniMaxCount> uniFullEnvStretch = {};
  std::array<TBlock, FFMasterUniMaxCount> uniFullOscPhaseRand = {};
  std::array<TBlock, FFMasterUniMaxCount> uniFullOscPhaseOffset = {};
  std::array<TBlock, FFMasterUniMaxCount> uniFullLFOPhaseRand = {};
  std::array<TBlock, FFMasterUniMaxCount> uniFullLFOPhaseOffset = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFMasterAccParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TAccurate, 1> modWheel = {};
  std::array<TAccurate, 1> pitchBend = {};
  std::array<TAccurate, FFMasterAuxCount> aux = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullOscPan = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullOscGain = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullOscFine = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullOscCoarse = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullLFORate = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullLFOMin = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullLFOMax = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullLFOSkewAX = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullLFOSkewAY = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullVFXParamA = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullVFXParamB = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullVFXParamC = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullVFXParamD = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoTapLevel = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoTapDelay = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoTapBal = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoFdbkDelay = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoFdbkMix = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoFdbkAmt = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoReverbMix = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoReverbSize = {};
  std::array<TAccurate, FFMasterUniMaxCount> uniFullEchoReverbDamp = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFMasterParamState final
{
  friend class FFPlugProcessor;
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FFMasterAccParamState<TAccurate> acc = {};
  FFMasterBlockParamState<TBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};