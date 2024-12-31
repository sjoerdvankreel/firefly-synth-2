#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFPlugProcessor.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <cmath>
#include <numbers>

FFPlugProcessor::
FFPlugProcessor(
FBStaticTopo const& topo, FFProcState* state, float sampleRate) :
_topo(topo),
_state(state),
_sampleRate(sampleRate) {}

FFModuleProcState 
FFPlugProcessor::MakeModuleState(FBPlugInputBlock const& input) const
{
  FFModuleProcState result = {};
  result.proc = _state;
  result.topo = &_topo;
  result.input = &input;
  result.sampleRate = _sampleRate;
  return result;
}

void 
FFPlugProcessor::LeaseVoices(FBPlugInputBlock const& input)
{
  for (int n = 0; n < input.note->size(); n++)
    if ((*input.note)[n].on)
      input.voiceManager->Lease((*input.note)[n]);
}

void 
FFPlugProcessor::ReturnVoices(FBPlugInputBlock const& input)
{
  for (int n = 0; n < input.note->size(); n++)
    if (!(*input.note)[n].on)
      input.voiceManager->ReturnOldest((*input.note)[n]);
}

void
FFPlugProcessor::ProcessPreVoice(
  FBPlugInputBlock const& input)
{
  auto moduleState = MakeModuleState(input);
  for (int s = 0; s < FFGLFOCount; s++)
  {
    moduleState.moduleSlot = s;
    _state->dsp.global.gLFO[s].processor.Process(moduleState);
  }
}

void
FFPlugProcessor::ProcessPostVoice(
  FBPlugInputBlock const& input, FBFixedAudioBlock& output)
{
  auto& gGilterIn = _state->dsp.global.gFilter[0].input;
  gGilterIn.Clear();
  for (int v = 0; v < FBMaxVoices; v++)
    if (input.voiceManager->IsActive(v))
      gGilterIn.Add(_state->dsp.voice[v].output);

  auto moduleState = MakeModuleState(input);
  moduleState.moduleSlot = 0;
  _state->dsp.global.gFilter[0].processor.Process(moduleState);
  for (int s = 1; s < FFGFilterCount; s++)
  {
    moduleState.moduleSlot = s;
    _state->dsp.global.gFilter[s].input.CopyFrom(_state->dsp.global.gFilter[s - 1].output);
    _state->dsp.global.gFilter[s].processor.Process(moduleState);
  }

  moduleState.moduleSlot = 0;
  _state->dsp.global.master.input.CopyFrom(_state->dsp.global.gFilter[FFGFilterCount - 1].output);
  _state->dsp.global.master.processor.Process(moduleState);
  output.CopyFrom(_state->dsp.global.master.output);
}

void
FFPlugProcessor::ProcessVoice(FBPlugInputBlock const& input, int voice)
{
  auto moduleState = MakeModuleState(input);
  moduleState.voice = &input.voiceManager->Voices()[voice];

  auto& voiceDSP = _state->dsp.voice[voice];
  voiceDSP.output.Clear();
  for (int i = 0; i < FFOsciCount; i++)
  {
    moduleState.moduleSlot = i;
    voiceDSP.osci[i].processor.Process(moduleState, voice);
    voiceDSP.output.Add(voiceDSP.osci[i].output);
  }
}