#pragma once

template <class TAccurate>
class alignas(alignof(TAccurate)) FFOsciParamStateVoiceStart final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeOsciTopo();
  std::array<TAccurate, 1> waveDSFBW = {};
  std::array<TAccurate, 1> uniOffset = {};
  std::array<TAccurate, 1> uniRandom = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamStateVoiceStart);
};