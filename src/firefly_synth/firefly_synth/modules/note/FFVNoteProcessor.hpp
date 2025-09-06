#pragma once

struct FBModuleProcState;

class FFVNoteProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVNoteProcessor);
  void BeginVoice(FBModuleProcState& state);
};