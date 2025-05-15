#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_plug/modules/noise/FFNoiseProcessor.hpp>

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFNoiseDSPState final
{
  friend class FFVoiceProcessor;
  friend struct NoiseGraphRenderData;
  std::unique_ptr<FFNoiseProcessor> processor = {};
public:
  FFNoiseDSPState();
  ~FFNoiseDSPState();
  FBSIMDArray2<float, FBFixedBlockSamples, 2> output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFNoiseDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFNoiseBlockParamState final
{
  friend class FFNoiseProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeNoiseTopo();
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> seed = {};
  std::array<TVoiceBlock, 1> poles = {};
  std::array<TVoiceBlock, 1> uniCount = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFNoiseBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFNoiseAccParamState final
{
  friend class FFNoiseProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeNoiseTopo();
  std::array<TVoiceAcc, 1> coarse = {};
  std::array<TVoiceAcc, 1> fine = {};
  std::array<TVoiceAcc, 1> gain = {};
  std::array<TVoiceAcc, 1> uniBlend = {};
  std::array<TVoiceAcc, 1> uniDetune = {};
  std::array<TVoiceAcc, 1> uniSpread = {};
  std::array<TVoiceAcc, 1> x = {};
  std::array<TVoiceAcc, 1> y = {};
  std::array<TVoiceAcc, 1> color = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFNoiseAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFNoiseParamState final
{
  friend class FFNoiseProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeNoiseTopo();
  FFNoiseAccParamState<TVoiceAcc> acc = {};
  FFNoiseBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFNoiseParamState);
};