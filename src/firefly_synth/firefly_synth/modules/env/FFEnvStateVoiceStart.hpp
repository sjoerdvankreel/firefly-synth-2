#pragma once

template <class TAccurate>
class alignas(alignof(TAccurate)) FFEnvVoiceStartParamState final
{
  friend class FFEnvProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeEnvTopo();
  std::array<TAccurate, FFEnvStageCount> stageTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvVoiceStartParamState);
};