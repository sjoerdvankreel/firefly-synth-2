#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFPlugProcessor.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>

#include <cmath>
#include <numbers>

FFPlugProcessor::
FFPlugProcessor(
FBRuntimeTopo const* topo, FFProcState* state, float sampleRate) :
_state(state),
_sampleRate(sampleRate),
_topo(topo) {}

FFModuleProcState 
FFPlugProcessor::MakeModuleState(
  FBPlugInputBlock const& input) const
{
  FFModuleProcState result = {};
  result.proc = _state;
  result.input = &input;
  result.topo = &_topo->static_;
  result.sampleRate = _sampleRate;
  return result;
}

FFModuleProcState
FFPlugProcessor::MakeModuleVoiceState(
  FBPlugInputBlock const& input, int voice) const
{
  auto result = MakeModuleState(input);
  result.voice = &result.input->voiceManager->Voices()[voice];
  return result;
}

void
FFPlugProcessor::ProcessVoice(
  FBPlugInputBlock const& input, int voice)
{
  auto state = MakeModuleVoiceState(input, voice);
  if(_state->dsp.voice[voice].processor.Process(state))
    input.voiceManager->Return(voice);
}

void 
FFPlugProcessor::LeaseVoices(
  FBPlugInputBlock const& input)
{
  for (int n = 0; n < input.note->size(); n++)
    if ((*input.note)[n].on)
    {
      int voice = input.voiceManager->Lease((*input.note)[n]);
      auto state = MakeModuleVoiceState(input, voice);
      _state->dsp.voice[voice].processor.BeginVoice(state);
    }
}

void
FFPlugProcessor::ProcessPreVoice(
  FBPlugInputBlock const& input)
{
  auto state = MakeModuleState(input);
  for (int s = 0; s < FFGLFOCount; s++)
  {
    state.moduleSlot = s;
    _state->dsp.global.gLFO[s].processor.Process(state);
  }
}

void
FFPlugProcessor::ProcessPostVoice(
  FBPlugInputBlock const& input, FBFixedOutputBlock& output)
{
  auto& gGilterIn = _state->dsp.global.gFilter[0].input;
  gGilterIn.Clear();
  for (int v = 0; v < FBMaxVoices; v++)
    if (input.voiceManager->IsActive(v))
      gGilterIn.Add(_state->dsp.voice[v].output);

  auto state = MakeModuleState(input);
  state.moduleSlot = 0;
  _state->dsp.global.gFilter[0].processor.Process(state);
  for (int s = 1; s < FFGFilterCount; s++)
  {
    state.moduleSlot = s;
    _state->dsp.global.gFilter[s].input.CopyFrom(_state->dsp.global.gFilter[s - 1].output);
    _state->dsp.global.gFilter[s].processor.Process(state);
  }

  state.moduleSlot = 0;
  _state->dsp.global.master.input.CopyFrom(_state->dsp.global.gFilter[FFGFilterCount - 1].output);
  _state->dsp.global.master.processor.Process(state);
  output.audio.CopyFrom(_state->dsp.global.master.output);
}