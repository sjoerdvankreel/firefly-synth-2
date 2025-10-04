#pragma once

#include <firefly_synth/modules/mix/FFVMixTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/note/FFVNoteTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <array>

struct FBPlugInputBlock;

class FFVoiceProcessor final
{
  FFVEchoTarget _vEchoTarget = {};
  FFVMixAmpEnvTarget _ampEnvTarget = {};

  // Need this to apply on-note modulation with sample accuracy.
  bool _firstRoundThisVoice = true;

  // Perf opt: we don't need to keep running the oscis
  // once the per-voice amplitude envelope is done.
  // This matters when using per-voice echo, which
  // extends the lifetime of the voice after VAMP.
  // The placement of VAMP is user-selectable, but
  // all of them are AFTER the oscis. So in any case,
  // once VAMP is done, osci output is zero.
  // It is tempting to think that after VAMP, we
  // don't run the LFO's and other ENV's anymore,
  // but -- not possible, because these are valid
  // modulation sources for the per-voice echo.
  // By the same logic, we can skip VFX after VAMP.
  // This short-circuits some extreme waveshaping
  // which turns silence into DC, but i don't consider
  // that a real use case.
  bool _ampEnvFinishedThisRound = false;
  bool _ampEnvFinishedPrevRound = false;

  std::array<float, FFVNoteOnNoteRandomCount> _onNoteRandomUni = {};
  std::array<float, FFVNoteOnNoteRandomCount> _onNoteRandomNorm = {};
  std::array<float, FFVNoteOnNoteRandomCount> _onNoteGroupRandomUni = {};
  std::array<float, FFVNoteOnNoteRandomCount> _onNoteGroupRandomNorm = {};
  std::array<float, (int)FFGlobalUniTarget::Count> _globalUniAutoRandState = {};

  bool ProcessVEcho(
    FBModuleProcState& state, int ampEnvFinishedAt, 
    FBSArray2<float, FBFixedBlockSamples, 2>& inout);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceProcessor);
  std::array<float, (int)FFGlobalUniTarget::Count> const& 
  GlobalUniAutoRandState() const { return _globalUniAutoRandState; }

  bool Process(FBModuleProcState state, int releaseAt);
  void BeginVoice(FBModuleProcState state,
    std::array<float, FFVNoteOnNoteRandomCount> const& onNoteRandomUni,
    std::array<float, FFVNoteOnNoteRandomCount> const& onNoteRandomNorm,
    std::array<float, FFVNoteOnNoteRandomCount> const& onNoteGroupRandomUni,
    std::array<float, FFVNoteOnNoteRandomCount> const& onNoteGroupRandomNorm);
};