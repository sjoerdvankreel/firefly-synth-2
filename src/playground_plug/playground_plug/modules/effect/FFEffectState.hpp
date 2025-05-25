#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>
#include <playground_plug/modules/effect/FFEffectProcessor.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFEffectDSPState final
{
  friend class FFVoiceProcessor;
  friend struct EffectGraphRenderData;
  std::unique_ptr<FFEffectProcessor> processor = {};
public:
  FFEffectDSPState();
  ~FFEffectDSPState();
  FBSArray2<float, FBFixedBlockSamples, 2> input = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEffectDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFEffectBlockParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
  std::array<TVoiceBlock, 1> type = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEffectAccParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
  std::array<TVoiceAcc, 1> shaperGain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEffectParamState final
{
  friend class FFEffectProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
  FFEffectAccParamState<TVoiceAcc> acc = {};
  FFEffectBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEffectParamState);
};