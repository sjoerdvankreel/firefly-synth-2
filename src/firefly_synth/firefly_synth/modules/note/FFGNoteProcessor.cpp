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
  auto* procState = state.ProcAs<FFProcState>();
  state.input->keyMatrix.CopyTo(procState->dsp.global.gNote.outputKeyMatrix);
  procState->dsp.global.gNote.outputKeyMatrix.DivKeyBy(127.0f);
}