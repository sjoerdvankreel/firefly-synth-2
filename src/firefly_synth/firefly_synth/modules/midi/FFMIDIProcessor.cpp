#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/midi/FFMIDIProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

void
FFMIDIProcessor::Process(FBModuleProcState& state)
{
  auto* container = state.input->procState;
  auto* procState = state.ProcAs<FFProcState>();
  container->MIDIParams()[FBMIDIEvent::CPMessageId].Global().CV().CopyTo(procState->dsp.global.midi.outputCP);
  container->MIDIParams()[FBMIDIEvent::PBMessageId].Global().CV().CopyTo(procState->dsp.global.midi.outputPB);
  for(int i = 0; i < FBMIDIEvent::CCMessageCount; i++)
    container->MIDIParams()[i].Global().CV().CopyTo(procState->dsp.global.midi.outputCC[i]);
}