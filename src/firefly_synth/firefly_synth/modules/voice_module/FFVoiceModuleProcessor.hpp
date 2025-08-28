#pragma once

#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFVoiceModuleProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceModuleProcessor);
  void Process(FBModuleProcState& state);
};