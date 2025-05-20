#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModState.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFOsciDSPState final
{
  friend class FFVoiceProcessor;
  friend struct OscisGraphRenderData;
  std::unique_ptr<FFOsciProcessor> processor = {};
public:
  FFOsciDSPState();
  ~FFOsciDSPState();
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FBSArray2<float, FFOsciFixedBlockOversamples, FFOsciBaseUniMaxCount> uniOutputOversampled = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> uniCount = {};
  std::array<TVoiceBlock, 1> uniOffset = {};
  std::array<TVoiceBlock, 1> uniRandom = {};
  std::array<TVoiceBlock, 1> waveHSMode = {};
  std::array<TVoiceBlock, FFOsciWavePWCount> wavePWMode = {};
  std::array<TVoiceBlock, FFOsciWaveBasicCount> waveBasicMode = {};
  std::array<TVoiceBlock, 1> waveDSFBW = {};
  std::array<TVoiceBlock, 1> waveDSFMode = {};
  std::array<TVoiceBlock, 1> waveDSFOver = {};
  std::array<TVoiceBlock, 1> waveDSFDistance = {};
  std::array<TVoiceBlock, 1> fmMode = {};
  std::array<TVoiceBlock, 1> fmRatioMode = {};
  std::array<TVoiceBlock, FFOsciFMOperatorCount - 1> fmRatioRatio = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAccParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TVoiceAcc, 1> coarse = {};
  std::array<TVoiceAcc, 1> fine = {};
  std::array<TVoiceAcc, 1> gain = {};
  std::array<TVoiceAcc, 1> uniBlend = {};
  std::array<TVoiceAcc, 1> uniDetune = {};
  std::array<TVoiceAcc, 1> uniSpread = {};
  std::array<TVoiceAcc, 1> waveHSGain = {};
  std::array<TVoiceAcc, 1> waveHSSync = {};
  std::array<TVoiceAcc, 1> waveDSFGain = {};
  std::array<TVoiceAcc, 1> waveDSFDecay = {};
  std::array<TVoiceAcc, FFOsciWavePWCount> wavePWPW = {};
  std::array<TVoiceAcc, FFOsciWavePWCount> wavePWGain = {};
  std::array<TVoiceAcc, FFOsciWaveBasicCount> waveBasicGain = {};
  std::array<TVoiceAcc, FFOsciFMMatrixSize> fmIndex = {};
  std::array<TVoiceAcc, FFOsciFMOperatorCount - 1> fmRatioFree = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  FFOsciAccParamState<TVoiceAcc> acc = {};
  FFOsciBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};