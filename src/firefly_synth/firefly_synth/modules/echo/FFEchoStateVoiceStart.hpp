#pragma once

template <bool Global>
class FFEchoProcessor;

template <class TAccurate>
class alignas(alignof(TAccurate)) FFEchoVoiceStartParamState final
{
  friend class FFPlugProcessor;
  friend class FFVoiceProcessor;
  friend class FFEchoProcessor<true>;
  friend class FFEchoProcessor<false>;
  friend std::unique_ptr<FBStaticModule> FFMakeEchoTopo(bool global);
  std::array<TAccurate, 1> voiceFadeTime = {};
  std::array<TAccurate, 1> voiceExtendTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEchoVoiceStartParamState);
};