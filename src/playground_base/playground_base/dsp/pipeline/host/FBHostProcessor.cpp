#include <playground_base/base/shared/FBDenormal.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/host/FBHostOutputBlock.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessContext.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBHostBufferProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <utility>

FBHostProcessor::
~FBHostProcessor() {}

FBHostProcessor::
FBHostProcessor(
  IFBHostProcessContext* hostContext,
  std::unique_ptr<IFBPlugProcessor>&& plug,
  FBProcStateContainer* state, float sampleRate):
_sampleRate(sampleRate),
_state(state),
_hostContext(hostContext),
_plug(std::move(plug)),
_voiceManager(std::make_unique<FBVoiceManager>(state)),
_smooth(std::make_unique<FBSmoothProcessor>(_voiceManager.get())),
_hostBuffer(std::make_unique<FBHostBufferProcessor>()),
_fixedBuffer(std::make_unique<FBFixedBufferProcessor>(_voiceManager.get()))
{
  _fixedOut.state = state;
  _plugIn.voiceManager = _voiceManager.get();
}

void
FBHostProcessor::ProcessAllVoices()
{
  for (int v = 0; v < FBMaxVoices; v++)
    ProcessVoice(v);
}

void 
FBHostProcessor::ProcessVoice(int slot)
{
  if (_plugIn.voiceManager->IsActive(slot))
    _plug->ProcessVoice(_plugIn, slot);
}

void 
FBHostProcessor::ProcessHost(
  FBHostInputBlock const& input, FBHostOutputBlock& output)
{
  auto denormalState = FBDisableDenormal(); 
  for (auto const& be : input.block)
    _fixedOut.state->Params()[be.param].Value(be.normalized);

  // TODO norm to plain
  _state->SetSmoothingCoeffs(_sampleRate, _state->Special().smooth->Value());

  FBFixedInputBlock const* fixedIn;
  _hostBuffer->BufferFromHost(input);
  while ((fixedIn = _hostBuffer->ProcessToFixed()) != nullptr)
  {
    _plugIn.note = &fixedIn->note;
    _plugIn.audio = &fixedIn->audio;
    _plug->LeaseVoices(_plugIn);
    _smooth->ProcessSmoothing(*fixedIn, _fixedOut);
    _plug->ProcessPreVoice(_plugIn);
    _hostContext->ProcessVoices();
    _plug->ProcessPostVoice(_plugIn, _fixedOut.audio);
    _plug->ReturnVoices(_plugIn);
    _fixedBuffer->BufferFromFixed(_fixedOut.audio);
  }
  _fixedBuffer->ProcessToHost(output);  
  FBRestoreDenormal(denormalState);
}