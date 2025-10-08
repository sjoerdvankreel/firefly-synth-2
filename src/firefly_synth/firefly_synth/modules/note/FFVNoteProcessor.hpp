#pragma once

#include <firefly_synth/modules/note/FFVNoteTopo.hpp>
#include <array>

struct FBModuleProcState;

class FFVNoteProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVNoteProcessor);
  void BeginVoice(
    FBModuleProcState& state,
    std::array<float, FFVNoteOnNoteRandomCount> const& randomUni,
    std::array<float, FFVNoteOnNoteRandomCount> const& randomNorm,
    std::array<float, FFVNoteOnNoteRandomCount> const& groupRandomUni,
    std::array<float, FFVNoteOnNoteRandomCount> const& groupRandomNorm);
};