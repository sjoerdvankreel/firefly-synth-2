#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBHostBufferProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <utility>

FBHostProcessor::
~FBHostProcessor() {}

FBHostProcessor::
FBHostProcessor(
  std::unique_ptr<IFBPlugProcessor>&& plug,
  FBProcStatePtrs const* state, float sampleRate):
_state(state),
_plug(std::move(plug)),
_voiceManager(std::make_unique<FBVoiceManager>(state)),
_smooth(std::make_unique<FBSmoothProcessor>(_voiceManager.get())),
_hostBuffer(std::make_unique<FBHostBufferProcessor>()),
_fixedBuffer(std::make_unique<FBFixedBufferProcessor>())
{
  _fixedOut.state = state;
  _plugIn.voiceManager = _voiceManager.get();
  for (int p = 0; p < state->isAcc.size(); p++)
    if (state->isAcc[p])
      if(!state->isVoice[p])
        state->globalAcc[p]->proc.smoother = 
          FBOnePoleFilter(sampleRate, FB_PARAM_SMOOTH_SEC);
      else
        for(int v = 0; v < FB_MAX_VOICES; v++)
          state->voiceAcc[p]->proc[v].smoother = 
            FBOnePoleFilter(sampleRate, FB_PARAM_SMOOTH_SEC);
}

void 
FBHostProcessor::ProcessVoices()
{
  auto& state = *_fixedOut.state;
  auto const& voices = _plugIn.voiceManager->Voices();
  for (int v = 0; v < voices.size(); v++)
    if (voices[v].active)
      _plug->ProcessVoice(_plugIn, v);
}

void 
FBHostProcessor::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.block)
    *_fixedOut.state->allBlock[be.index] = be.normalized;

  FBFixedInputBlock const* fixedIn;
  _hostBuffer->BufferFromHost(input);
  while ((fixedIn = _hostBuffer->ProcessToFixed()) != nullptr)
  {
    _plugIn.note = &fixedIn->note;
    _plugIn.audio = &fixedIn->audio;
    _smooth->ProcessSmoothing(*fixedIn, _fixedOut);
    _plug->ProcessPreVoice(_plugIn);
    ProcessVoices();
    _plug->ProcessPostVoice(_plugIn, _fixedOut.audio);
    _fixedBuffer->BufferFromFixed(_fixedOut.audio);
  }
  _fixedBuffer->ProcessToHost(output);
}