#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBAccParamState.hpp>
#include <firefly_base/base/state/proc/FBVoiceAccParamState.hpp>
#include <firefly_base/base/state/proc/FBGlobalAccParamState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBModuleExchangeState.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/dsp/plug/FBPlugProcessor.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>
#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/dsp/host/FBHostDSPContext.hpp>
#include <firefly_base/dsp/host/FBSmoothingProcessor.hpp>
#include <firefly_base/dsp/buffer/FBHostToPlugProcessor.hpp>
#include <firefly_base/dsp/buffer/FBPlugToHostProcessor.hpp>

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
  _lastMIDINoteKey = 60.0f;
  _plugOut.procState = _procState;
  _plugIn.lastMIDINoteKey.Fill(60.0f);
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
  _plugIn.projectTimeSamples = input.projectTimeSamples;

  auto denormalState = FBDisableDenormal(); 
  for (auto const& be : input.blockAuto)
    _procState->Params()[be.param].Value(be.normalized);

  auto const& hostSmoothTimeSpecial = _procState->Special().hostSmoothTime;
  auto const& hostSmoothTimeTopo = hostSmoothTimeSpecial.ParamTopo(_topo->static_);
  int hostSmoothSamples = hostSmoothTimeTopo.Linear().NormalizedTimeToSamplesFast(hostSmoothTimeSpecial.state->Value(), _sampleRate);
  _procState->SetSmoothingCoeffs(hostSmoothSamples);

  for (int m = 0; m < _topo->modules.size(); m++)
  {
    auto const& indices = _topo->modules[m].topoIndices;
    auto const& static_ = _topo->static_.modules[indices.index];
    if (!static_.voice)
    {
      if (_exchangeState->Modules()[m] != nullptr)
        _exchangeState->Modules()[m]->Global()->active = false;
    }
    else
    {
      if (_exchangeState->Modules()[m] != nullptr)
        for (int v = 0; v < FBMaxVoices; v++)
        _exchangeState->Modules()[m]->Voice()[v]->active = false;
    }
  }

  FBFixedInputBlock* fixedIn;
  _hostToPlug->BufferFromHost(input);
  while ((fixedIn = _hostToPlug->ProcessToPlug()) != nullptr)
  {
    _plugIn.audio = &fixedIn->audio;
    _plugIn.noteEvents = &fixedIn->noteEvents;
    
    int n = 0;
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      for (; n < _plugIn.noteEvents->size() && (*_plugIn.noteEvents)[n].pos == s; n++)
        if((*_plugIn.noteEvents)[n].on)
          _lastMIDINoteKey = static_cast<float>((*_plugIn.noteEvents)[n].note.key);
      _plugIn.lastMIDINoteKey.Set(s, _lastMIDINoteKey);
    }

    // Sets voice offsetInBlock to possibly nonzero.
    _plug->LeaseVoices(_plugIn);
    _smoothing->ProcessSmoothing(*fixedIn, _plugOut, hostSmoothSamples);
    _plug->ProcessPreVoice(_plugIn);

    ProcessVoices();

    // Voice offsetInBlock is 0 for the rest of the voice lifetime.
    for (int v = 0; v < FBMaxVoices; v++)
      if (_voiceManager->IsActive(v))
        _voiceManager->_voices[v].offsetInBlock = 0;

    _plug->ProcessPostVoice(_plugIn, _plugOut);
    _plugToHost->BufferFromPlug(_plugOut.audio);
    _plugIn.projectTimeSamples += FBFixedBlockSamples;
  }
  _plugToHost->ProcessToHost(output);

  for (auto const& entry : _plugOut.outputParamsNormalized)
    output.outputParams.push_back({ entry.first, entry.second });

  _exchangeState->Host()->bpm = input.bpm;
  _exchangeState->Host()->sampleRate = _sampleRate;
  _exchangeState->Host()->lastMIDINoteKey = _plugIn.lastMIDINoteKey.Last();

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