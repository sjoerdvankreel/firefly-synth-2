#pragma once

#include <playground_plug/modules/env/FFEnvProcessor.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(sizeof(FBFloatVector)) FFEnvDSPState final
{
  friend class FFVoiceProcessor;
  FFEnvProcessor processor = {};
public:
  FBFixedFloatBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvDSPState);
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
  std::array<TVoiceBlock, 1> delayTime = {};
  std::array<TVoiceBlock, 1> attackTime = {};
  std::array<TVoiceBlock, 1> holdTime = {};
  std::array<TVoiceBlock, 1> decayTime = {};
  std::array<TVoiceBlock, 1> releaseTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEnvAccParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  std::array<TVoiceAcc, 1> smooth = {};
  std::array<TVoiceAcc, 1> sustain = {};
  std::array<TVoiceAcc, 1> decaySlope = {};
  std::array<TVoiceAcc, 1> attackSlope = {};
  std::array<TVoiceAcc, 1> releaseSlope = {};
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