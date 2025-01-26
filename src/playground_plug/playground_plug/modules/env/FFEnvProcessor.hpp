#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;
enum class FFEnvStage { Delay, Attack, Hold, Decay, Sustain, Release };

class FFEnvProcessor final
{
  int _position = 0;
  FFEnvStage _stage = FFEnvStage::Delay;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  void Process(FFModuleProcState const& state);
  void BeginVoice(FFModuleProcState const& state);
};