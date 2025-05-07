#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/base/state/exchange/FBModuleProcExchangeState.hpp>

#include <array>
#include <memory>

class FFEnvProcessor;
struct FBStaticModule;
struct FBModuleGraphComponentData;

struct FFEnvExchangeState final:
public FBModuleProcExchangeState
{
  float lastOutput = {};
};

class FFEnvGUIState final
{
  friend struct FFGUIState;
  std::array<double, 1> graphKeyTimePct = {};
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvGUIState);
};

class alignas(FBSIMDAlign) FFEnvDSPState final
{
  friend class FFVoiceProcessor;
  friend struct EnvGraphRenderData;
  std::unique_ptr<FFEnvProcessor> processor = {};
public:
  FFEnvDSPState();
  ~FFEnvDSPState();
  FBFixedFloatArray output = {};
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEnvDSPState);
}; 

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFEnvBlockParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> type = {};
  std::array<TVoiceBlock, 1> mode = {};
  std::array<TVoiceBlock, 1> sync = {};
  std::array<TVoiceBlock, 1> holdTime = {};
  std::array<TVoiceBlock, 1> holdBars = {};
  std::array<TVoiceBlock, 1> delayTime = {};
  std::array<TVoiceBlock, 1> delayBars = {};
  std::array<TVoiceBlock, 1> decayTime = {};
  std::array<TVoiceBlock, 1> decayBars = {};
  std::array<TVoiceBlock, 1> attackTime = {};
  std::array<TVoiceBlock, 1> attackBars = {};
  std::array<TVoiceBlock, 1> smoothTime = {};
  std::array<TVoiceBlock, 1> smoothBars = {};
  std::array<TVoiceBlock, 1> releaseTime = {};
  std::array<TVoiceBlock, 1> releaseBars = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEnvAccParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  std::array<TVoiceAcc, 1> decaySlope = {};
  std::array<TVoiceAcc, 1> attackSlope = {};
  std::array<TVoiceAcc, 1> releaseSlope = {};
  std::array<TVoiceAcc, 1> sustainLevel = {};
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