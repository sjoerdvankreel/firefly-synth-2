#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
#include <firefly_synth/modules/osci/FFOsciStateVoiceStart.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModState.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFOsciDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend struct OsciGraphRenderData;
  std::unique_ptr<FFOsciProcessor> processor = {};
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciDSPState);
  FFOsciDSPState(): processor(std::make_unique<FFOsciProcessor>()) {}
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciUniMaxCount> uniOutputOversampled = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TBlock, 1> type = {};
  std::array<TBlock, 1> uniCount = {};
  std::array<TBlock, 1> waveHSMode = {};
  std::array<TBlock, FFOsciWavePWCount> wavePWMode = {};
  std::array<TBlock, FFOsciWaveBasicCount> waveBasicMode = {};
  std::array<TBlock, 1> waveDSFBW = {};
  std::array<TBlock, 1> waveDSFMode = {};
  std::array<TBlock, 1> waveDSFOver = {};
  std::array<TBlock, 1> waveDSFDistance = {};
  std::array<TBlock, 1> fmMode = {};
  std::array<TBlock, 1> fmRatioMode = {};
  std::array<TBlock, FFOsciFMOperatorCount - 1> fmRatioRatio = {};
  std::array<TBlock, 1> stringMode = {};
  std::array<TBlock, 1> stringLPOn = {};
  std::array<TBlock, 1> stringHPOn = {};
  std::array<TBlock, 1> stringSeed = {};
  std::array<TBlock, 1> stringPoles = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFOsciAccParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TAccurate, 1> pan = {};
  std::array<TAccurate, 1> gain = {};
  std::array<TAccurate, 1> envToGain = {};
  std::array<TAccurate, 1> fine = {};
  std::array<TAccurate, 1> lfoToFine = {};
  std::array<TAccurate, 1> coarse = {};
  std::array<TAccurate, 1> uniBlend = {};
  std::array<TAccurate, 1> uniDetune = {};
  std::array<TAccurate, 1> uniSpread = {};
  std::array<TAccurate, 1> waveHSGain = {};
  std::array<TAccurate, 1> waveHSPitch = {};
  std::array<TAccurate, 1> waveDSFGain = {};
  std::array<TAccurate, 1> waveDSFDecay = {};
  std::array<TAccurate, FFOsciWavePWCount> wavePWPW = {};
  std::array<TAccurate, FFOsciWavePWCount> wavePWGain = {};
  std::array<TAccurate, FFOsciWaveBasicCount> waveBasicGain = {};
  std::array<TAccurate, FFOsciFMMatrixSize> fmIndex = {};
  std::array<TAccurate, FFOsciFMOperatorCount - 1> fmRatioFree = {};
  std::array<TAccurate, 1> stringX = {};
  std::array<TAccurate, 1> stringY = {};
  std::array<TAccurate, 1> stringColor = {};
  std::array<TAccurate, 1> stringExcite = {};
  std::array<TAccurate, 1> stringLPRes = {};
  std::array<TAccurate, 1> stringHPRes = {};
  std::array<TAccurate, 1> stringLPFreq = {};
  std::array<TAccurate, 1> stringHPFreq = {};
  std::array<TAccurate, 1> stringLPKTrk = {};
  std::array<TAccurate, 1> stringHPKTrk = {};
  std::array<TAccurate, 1> stringDamp = {};
  std::array<TAccurate, 1> stringDampKTrk = {};
  std::array<TAccurate, 1> stringFeedback = {};
  std::array<TAccurate, 1> stringFeedbackKTrk = {};
  std::array<TAccurate, 1> stringTrackingKey = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFOsciParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  FFOsciAccParamState<TAccurate> acc = {};
  FFOsciBlockParamState<TBlock> block = {};
  FFOsciParamStateVoiceStart<TAccurate> voiceStart = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};