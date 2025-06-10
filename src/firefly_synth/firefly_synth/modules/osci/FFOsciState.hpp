#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModState.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

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

template <class TBlock>
class alignas(alignof(TBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TBlock, 1> type = {};
  std::array<TBlock, 1> uniCount = {};
  std::array<TBlock, 1> uniOffset = {};
  std::array<TBlock, 1> uniRandom = {};
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
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFOsciAccParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TAccurate, 1> coarse = {};
  std::array<TAccurate, 1> fine = {};
  std::array<TAccurate, 1> gain = {};
  std::array<TAccurate, 1> uniBlend = {};
  std::array<TAccurate, 1> uniDetune = {};
  std::array<TAccurate, 1> uniSpread = {};
  std::array<TAccurate, 1> waveHSGain = {};
  std::array<TAccurate, 1> waveHSSync = {};
  std::array<TAccurate, 1> waveDSFGain = {};
  std::array<TAccurate, 1> waveDSFDecay = {};
  std::array<TAccurate, FFOsciWavePWCount> wavePWPW = {};
  std::array<TAccurate, FFOsciWavePWCount> wavePWGain = {};
  std::array<TAccurate, FFOsciWaveBasicCount> waveBasicGain = {};
  std::array<TAccurate, FFOsciFMMatrixSize> fmIndex = {};
  std::array<TAccurate, FFOsciFMOperatorCount - 1> fmRatioFree = {};
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
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};