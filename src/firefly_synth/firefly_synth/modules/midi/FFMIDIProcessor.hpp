#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <chrono>

struct FBModuleProcState;

class FFMIDIProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMIDIProcessor);
  void Process(FBModuleProcState& state);
};