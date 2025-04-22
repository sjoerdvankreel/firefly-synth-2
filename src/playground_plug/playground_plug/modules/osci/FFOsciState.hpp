#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

#include <playground_base/base/shared/FBMDArray.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;
class FFOsciProcessor;

inline int constexpr FFOsciOverSamplingFactor = 2;
inline int constexpr FFOsciOverSamplingTimes = 1 << FFOsciOverSamplingFactor;
typedef FBMDArray2< FBFixedFloatArray, FFOsciOverSamplingTimes, FFOsciUnisonMaxCount> FFOsciOversampledUnisonArray;

class alignas(FBSIMDAlign) FFOsciDSPState final
{
  friend class FFVoiceProcessor;
  friend struct OscisGraphRenderData;
  std::unique_ptr<FFOsciProcessor> processor = {};
public:
  FFOsciDSPState();
  ~FFOsciDSPState();
  FBFixedFloatAudioArray output = {};
  FFOsciOversampledUnisonArray unisonOutputMaybeOversampled = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> unisonCount = {};
  std::array<TVoiceBlock, 1> unisonOffset = {};
  std::array<TVoiceBlock, 1> unisonRandom = {};
  std::array<TVoiceBlock, 1> basicSinOn = {};
  std::array<TVoiceBlock, 1> basicSawOn = {};
  std::array<TVoiceBlock, 1> basicTriOn = {};
  std::array<TVoiceBlock, 1> basicSqrOn = {};
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
  std::array<TVoiceAcc, 1> gain = {};
  std::array<TVoiceAcc, 1> fine = {};
  std::array<TVoiceAcc, 1> sync = {};
  std::array<TVoiceAcc, 1> coarse = {};
  std::array<TVoiceAcc, 1> unisonBlend = {};
  std::array<TVoiceAcc, 1> unisonDetune = {};
  std::array<TVoiceAcc, 1> unisonSpread = {};
  std::array<TVoiceAcc, 1> dsfDecay = {};
  std::array<TVoiceAcc, 1> gLFOToGain = {};
  std::array<TVoiceAcc, 1> basicSqrPW = {};
  std::array<TVoiceAcc, 1> basicSinGain = {};
  std::array<TVoiceAcc, 1> basicSawGain = {};
  std::array<TVoiceAcc, 1> basicTriGain = {};
  std::array<TVoiceAcc, 1> basicSqrGain = {};
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