#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;

class FFGNoteProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGNoteProcessor);
  void Process(FBModuleProcState& state);
};