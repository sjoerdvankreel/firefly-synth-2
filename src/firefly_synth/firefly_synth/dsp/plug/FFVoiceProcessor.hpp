#pragma once

#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/note/FFVNoteTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <array>

struct FBPlugInputBlock;

class FFVoiceProcessor final
{
  bool _firstRoundThisVoice = true;
  std::array<float, FFVNoteOnNoteRandomCount> _onNoteRandomUni = {};
  std::array<float, FFVNoteOnNoteRandomCount> _onNoteRandomNorm = {};
  std::array<float, FFVNoteOnNoteRandomCount> _onNoteGroupRandomUni = {};
  std::array<float, FFVNoteOnNoteRandomCount> _onNoteGroupRandomNorm = {};
  
  FFVEchoTarget GetCurrentVEchoTarget(
    FBModuleProcState const& state);
  bool ProcessVEcho(
    FBModuleProcState& state, int ampEnvFinishedAt, 
    FBSArray2<float, FBFixedBlockSamples, 2>& inout);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceProcessor);
  bool Process(FBModuleProcState state, int releaseAt);
  void BeginVoice(FBModuleProcState state,
    std::array<float, FFVNoteOnNoteRandomCount> const& onNoteRandomUni,
    std::array<float, FFVNoteOnNoteRandomCount> const& onNoteRandomNorm,
    std::array<float, FFVNoteOnNoteRandomCount> const& onNoteGroupRandomUni,
    std::array<float, FFVNoteOnNoteRandomCount> const& onNoteGroupRandomNorm);
};