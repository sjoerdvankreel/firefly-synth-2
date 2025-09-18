#pragma once

template <class TAccurate>
class alignas(alignof(TAccurate)) FFVoiceModuleVoiceStartParamState final
{
  friend class FFVoiceModuleProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeVoiceModuleTopo();
  std::array<TAccurate, 1> portaTime = {};
  std::array<TAccurate, 1> portaSectionAmpAttack = {};
  std::array<TAccurate, 1> portaSectionAmpRelease = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceModuleVoiceStartParamState);
};