#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/note/FFGNoteProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

void
FFGNoteProcessor::Process(FBModuleProcState& state)
{
  auto* container = state.input->procState;
  auto* procState = state.ProcAs<FFProcState>();
  state.input->noteMatrixRaw->CopyTo(procState->dsp.global.gNote.outputNoteMatrixRaw);
  for(int i = 0; i < (int)FBNoteMatrixEntry::Count; i++)
    container->NoteMatrixSmth().entries[i].Global().CV().CopyTo(procState->dsp.global.gNote.outputNoteMatrixSmth.entries[i]);
}