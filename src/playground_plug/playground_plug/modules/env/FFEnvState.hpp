#pragma once

#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/state/exchange/FBModuleExchangeState.hpp>

#include <array>
#include <memory>

struct FBStaticModule;
struct FBModuleGraphComponentData;

class alignas(FBSIMDAlign) FFEnvDSPState final
{
  friend class FFVoiceProcessor;
  friend struct EnvGraphRenderData;
  std::unique_ptr<FFEnvProcessor> processor = {};
public:
  FFEnvDSPState();
  ~FFEnvDSPState();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEnvDSPState);
  FBSArray<float, FBFixedBlockSamples> output = {};
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFEnvBlockParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> exp = {};
  std::array<TVoiceBlock, 1> sync = {};
  std::array<TVoiceBlock, 1> sustain = {};
  std::array<TVoiceBlock, 1> release = {};
  std::array<TVoiceBlock, 1> loopStart = {};
  std::array<TVoiceBlock, 1> loopLength = {};
  std::array<TVoiceBlock, 1> smoothTime = {};
  std::array<TVoiceBlock, 1> smoothBars = {};
  std::array<TVoiceBlock, FFEnvStageCount> stageTime = {};
  std::array<TVoiceBlock, FFEnvStageCount> stageBars = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEnvAccParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  std::array<TVoiceAcc, FFEnvStageCount> stageSlope = {};
  std::array<TVoiceAcc, FFEnvStageCount> stageLevel = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEnvParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  FFEnvAccParamState<TVoiceAcc> acc = {};
  FFEnvBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvParamState);
};