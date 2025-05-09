#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>
#include <playground_base/base/state/proc/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/proc/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/exchange/FBModuleExchangeState.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/dsp/buffer/FBHostToPlugProcessor.hpp>
#include <playground_base/dsp/buffer/FBPlugToHostProcessor.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostDSPContext.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothingProcessor.hpp>

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
_hostToPlug(std::make_unique<FBHostToPlugProcessor>()),
_plugToHost(std::make_unique<FBPlugToHostProcessor>(_voiceManager.get())),
_smoothing(std::make_unique<FBSmoothingProcessor>(_voiceManager.get(), static_cast<int>(hostContext->ProcState()->Params().size())))
{
  _fixedOut.procState = _procState;
  _plugIn.sampleRate = _sampleRate;
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
  _plugIn.bpm = input.bpm;
  auto denormalState = FBDisableDenormal(); 
  for (auto const& be : input.blockAuto)
    _procState->Params()[be.param].Value(be.normalized);

  auto const& hostSmoothTimeSpecial = _procState->Special().hostSmoothTime;
  auto const& hostSmoothTimeTopo = hostSmoothTimeSpecial.ParamTopo(_topo->static_);
  int hostSmoothSamples = hostSmoothTimeTopo.Linear().NormalizedTimeToSamplesFast(hostSmoothTimeSpecial.state->Value(), _sampleRate);
  _procState->SetSmoothingCoeffs(hostSmoothSamples);

  _exchangeState->Host()->bpm = input.bpm;
  _exchangeState->Host()->sampleRate = _sampleRate;
  for (int m = 0; m < _topo->modules.size(); m++)
  {
    auto const& indices = _topo->modules[m].topoIndices;
    auto const& static_ = _topo->static_.modules[indices.index];
    if (!static_.voice)
    {
      if(_exchangeState->Modules()[m] != nullptr)
        *_exchangeState->Modules()[m]->Global() = {};
    }
    else
    {
      if (_exchangeState->Modules()[m] != nullptr)
        for (int v = 0; v < FBMaxVoices; v++)
        *_exchangeState->Modules()[m]->Voice()[v] = {};
    }
  }

  FBFixedInputBlock* fixedIn;
  _hostToPlug->BufferFromHost(input);
  while ((fixedIn = _hostToPlug->ProcessToPlug()) != nullptr)
  {
    _plugIn.note = &fixedIn->note;
    _plugIn.audio = &fixedIn->audio;
    _plug->LeaseVoices(_plugIn);
    _smoothing->ProcessSmoothing(*fixedIn, _fixedOut, hostSmoothSamples);
    _plug->ProcessPreVoice(_plugIn);
    ProcessVoices();
    _plug->ProcessPostVoice(_plugIn, _fixedOut);
    _plugToHost->BufferFromPlug(_fixedOut.audio);
  }
  _plugToHost->ProcessToHost(output);

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