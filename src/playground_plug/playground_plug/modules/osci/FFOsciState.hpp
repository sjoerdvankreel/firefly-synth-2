#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModState.hpp>

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBMDArray.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;
class FFOsciProcessor;

class alignas(FBSIMDAlign) FFOsciDSPState final
{
  friend class FFVoiceProcessor;
  friend struct OscisGraphRenderData;
  std::unique_ptr<FFOsciProcessor> processor = {};
public:
  FFOsciDSPState();
  ~FFOsciDSPState();
  FBSIMDArray2<float, FBFixedBlockSamples, 2> output = {};
  FBSIMDArray2<float, FFOsciFixedBlockOversamples, FFOsciUniMaxCount> uniOutputOversampled = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> uniCount = {};
  std::array<TVoiceBlock, 1> uniOffset = {};
  std::array<TVoiceBlock, 1> uniRandom = {};
  std::array<TVoiceBlock, FFOsciBasicCount> basicMode = {};
  std::array<TVoiceBlock, 1> dsfMode = {};
  std::array<TVoiceBlock, 1> dsfDistance = {};
  std::array<TVoiceBlock, 1> dsfOvertones = {};
  std::array<TVoiceBlock, 1> dsfBandwidth = {};
  std::array<TVoiceBlock, 1> fmExp = {};
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
  std::array<TVoiceAcc, 1> unisonBlend = {};
  std::array<TVoiceAcc, 1> unisonDetune = {};
  std::array<TVoiceAcc, 1> unisonSpread = {};
  std::array<TVoiceAcc, 1> dsfDecay = {};
  std::array<TVoiceAcc, FFOsciBasicCount> basicPW = {};
  std::array<TVoiceAcc, FFOsciBasicCount> basicGain = {};
  std::array<TVoiceAcc, FFOsciBasicCount> basicSync = {};
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