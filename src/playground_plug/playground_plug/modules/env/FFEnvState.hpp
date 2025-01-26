#pragma once

#include <playground_plug/modules/env/FFEnvProcessor.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(sizeof(FBFloatVector)) FFEnvDSPState final
{
  friend class FFPlugProcessor;
  FFEnvProcessor processor = {};
public:
  FBFixedFloatAudioBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvDSPState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFEnvBlockParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFEnvAccParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
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