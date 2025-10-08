#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/note/FFVNoteProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

void
FFVNoteProcessor::BeginVoice(
  FBModuleProcState& state,
  std::array<float, FFVNoteOnNoteRandomCount> const& randomUni,
  std::array<float, FFVNoteOnNoteRandomCount> const& randomNorm,
  std::array<float, FFVNoteOnNoteRandomCount> const& groupRandomUni,
  std::array<float, FFVNoteOnNoteRandomCount> const& groupRandomNorm)
  {
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  procState->dsp.voice[voice].vNote.outputVelo.Fill(state.voice->event.velo);
  procState->dsp.voice[voice].vNote.outputKeyUntuned.Fill(state.voice->event.note.keyUntuned / 127.0f);
  for(int i = 0; i < FFVNoteOnNoteRandomCount; i++)
  {
    procState->dsp.voice[voice].vNote.outputRandomUni[i].Fill(randomUni[i]);
    procState->dsp.voice[voice].vNote.outputRandomNorm[i].Fill(randomNorm[i]);
    procState->dsp.voice[voice].vNote.outputGroupRandomUni[i].Fill(groupRandomUni[i]);
    procState->dsp.voice[voice].vNote.outputGroupRandomNorm[i].Fill(groupRandomNorm[i]);
  }
}