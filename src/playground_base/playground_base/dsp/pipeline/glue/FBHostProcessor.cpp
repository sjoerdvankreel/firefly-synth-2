#include <playground_base/base/shared/FBDenormal.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>
#include <playground_base/base/state/proc/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/proc/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/exchange/FBModuleExchangeState.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>
#include <playground_base/base/state/exchange/FBModuleProcExchangeState.hpp>

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostDSPContext.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostOutputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothingProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBHostBufferProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <utility>

FBHostProcessor::
~FBHostProcessor() {}

FBHostProcessor::
FBHostProcessor(IFBHostDSPContext* hostContext):
_sampleRate(hostContext->SampleRate()),
_topo(hostContext->Topo()),
_procState(hostContext->ProcState()),
_exchangeState(hostContext->ExchangeState()),
_plug(hostContext->MakePlugProcessor()),
_voiceManager(std::make_unique<FBVoiceManager>(hostContext->ProcState())),
_hostBuffer(std::make_unique<FBHostBufferProcessor>()),
_fixedBuffer(std::make_unique<FBFixedBufferProcessor>(_voiceManager.get())),
_smoothing(std::make_unique<FBSmoothingProcessor>(_voiceManager.get(), (int)hostContext->ProcState()->Params().size()))
{
  _fixedOut.procState = _procState;
  _plugIn.voiceManager = _voiceManager.get();
}

void
FBHostProcessor::ProcessVoices()
{
  for (int v = 0; v < FBMaxVoices; v++)
    if (_plugIn.voiceManager->IsActive(v))
      _plug->ProcessVoice(_plugIn, v);
}

void 
FBHostProcessor::ProcessHost(
  FBHostInputBlock const& input, FBHostOutputBlock& output)
{
  auto denormalState = FBDisableDenormal(); 
  for (auto const& be : input.block)
    _procState->Params()[be.param].Value(be.normalized);

  auto const& hostSmoothTimeSpecial = _procState->Special().hostSmoothTime;
  auto const& hostSmoothTimeTopo = hostSmoothTimeSpecial.ParamTopo(_topo->static_);
  int hostSmoothSamples = hostSmoothTimeTopo.linear.NormalizedTimeToSamples(hostSmoothTimeSpecial.state->Value(), _sampleRate);
  _procState->SetSmoothingCoeffs(hostSmoothSamples);

  *_topo->static_.state.sampleRateExchangeAddr(_exchangeState->Raw()) = _sampleRate;
  for (int m = 0; m < _topo->modules.size(); m++)
  {
    auto const& indices = _topo->modules[m].topoIndices;
    auto const& static_ = _topo->static_.modules[indices.index];
    if (!static_.voice)
      *_exchangeState->Modules()[m].Global() = {};
    else
      for (int v = 0; v < FBMaxVoices; v++)
        *_exchangeState->Modules()[m].Voice()[v] = {};
  }

  FBFixedInputBlock const* fixedIn;
  _hostBuffer->BufferFromHost(input);
  while ((fixedIn = _hostBuffer->ProcessToFixed()) != nullptr)
  {
    _plugIn.note = &fixedIn->note;
    _plugIn.audio = &fixedIn->audio;
    _plug->LeaseVoices(_plugIn);
    _smoothing->ProcessSmoothing(*fixedIn, _fixedOut, hostSmoothSamples);
    _plug->ProcessPreVoice(_plugIn);
    ProcessVoices();
    _plug->ProcessPostVoice(_plugIn, _fixedOut);
    _fixedBuffer->BufferFromFixed(_fixedOut.audio);
  }
  _fixedBuffer->ProcessToHost(output);  

  for (auto const& entry : _fixedOut.outputParamsNormalized)
    output.outputParams.push_back({ entry.first, entry.second });

  for (int v = 0; v < FBMaxVoices; v++)
    _exchangeState->Voices()[v] = _voiceManager->Voices()[v];

  for (int i = 0; i < _procState->Params().size(); i++)
    if (!_procState->Params()[i].IsAcc())
      if (!_procState->Params()[i].IsVoice())
        *_exchangeState->Params()[i].Global() = 
          _procState->Params()[i].GlobalBlock().Value();
      else
        for (int v = 0; v < FBMaxVoices; v++)
          if (_voiceManager->IsActive(v))
            _exchangeState->Params()[i].Voice()[v] = 
              _procState->Params()[i].VoiceBlock().Voice()[v];

  FBRestoreDenormal(denormalState);
}