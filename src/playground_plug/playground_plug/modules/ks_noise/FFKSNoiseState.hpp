#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseTopo.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseProcessor.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFKSNoiseDSPState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend struct KSNoiseGraphRenderData;
  std::unique_ptr<FFKSNoiseProcessor> processor = {};
public:
  FFKSNoiseDSPState();
  ~FFKSNoiseDSPState();
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFKSNoiseDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFKSNoiseBlockParamState final
{
  friend class FFKSNoiseProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeKSNoiseTopo();
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> seed = {};
  std::array<TVoiceBlock, 1> poles = {};
  std::array<TVoiceBlock, 1> uniCount = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFKSNoiseBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFKSNoiseAccParamState final
{
  friend class FFKSNoiseProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeKSNoiseTopo();
  std::array<TVoiceAcc, 1> coarse = {};
  std::array<TVoiceAcc, 1> fine = {};
  std::array<TVoiceAcc, 1> gain = {};
  std::array<TVoiceAcc, 1> uniBlend = {};
  std::array<TVoiceAcc, 1> uniDetune = {};
  std::array<TVoiceAcc, 1> uniSpread = {};
  std::array<TVoiceAcc, 1> x = {};
  std::array<TVoiceAcc, 1> y = {};
  std::array<TVoiceAcc, 1> color = {};
  std::array<TVoiceAcc, 1> excite = {};
  std::array<TVoiceAcc, 1> exciteMix = {};
  std::array<TVoiceAcc, 1> range = {};
  std::array<TVoiceAcc, 1> center = {};
  std::array<TVoiceAcc, 1> damp = {};
  std::array<TVoiceAcc, 1> dampScale = {};
  std::array<TVoiceAcc, 1> feedback = {};
  std::array<TVoiceAcc, 1> feedbackScale = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFKSNoiseAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFKSNoiseParamState final
{
  friend class FFKSNoiseProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeKSNoiseTopo();
  FFKSNoiseAccParamState<TVoiceAcc> acc = {};
  FFKSNoiseBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFKSNoiseParamState);
};