#pragma once

template <class TAccurate>
class alignas(alignof(TAccurate)) FFLFOVoiceStartParamState final
{
  friend class FFLFOProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeLFOTopo(bool);
  std::array<TAccurate, FFLFOBlockCount> phase = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOVoiceStartParamState);
};