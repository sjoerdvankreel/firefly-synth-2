#pragma once

#include <firefly_synth/modules/env/FFEnvProcessor.hpp>
#include <firefly_synth/modules/env/FFEnvStateVoiceStart.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/exchange/FBModuleExchangeState.hpp>

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
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEnvDSPState);
  FFEnvDSPState() : processor(std::make_unique<FFEnvProcessor>()) {}
  FBSArray<float, FBFixedBlockSamples> output = {};
};

template <class TBlock>
class alignas(alignof(TBlock)) FFEnvBlockParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  std::array<TBlock, 1> type = {};
  std::array<TBlock, 1> sync = {};
  std::array<TBlock, 1> release = {};
  std::array<TBlock, 1> loopStart = {};
  std::array<TBlock, 1> loopLength = {};
  std::array<TBlock, 1> smoothTime = {};
  std::array<TBlock, 1> smoothBars = {};
  std::array<TBlock, FFEnvStageCount> stageBars = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvBlockParamState);
};

template <class TAccurate>
class alignas(alignof(TAccurate)) FFEnvAccParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  std::array<TAccurate, FFEnvStageCount> stageSlope = {};
  std::array<TAccurate, FFEnvStageCount> stageLevel = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvAccParamState);
};

template <class TBlock, class TAccurate>
class alignas(alignof(TAccurate)) FFEnvParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  FFEnvBlockParamState<TBlock> block = {};
  FFEnvAccParamState<TAccurate> acc = {};
  FFEnvVoiceStartParamState<TAccurate> voiceStart = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvParamState);
};