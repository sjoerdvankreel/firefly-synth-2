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

#include <chrono>
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
  _noteMatrix.SetKey(60.0f);
  _plugOut.procState = _procState;
  _plugIn.procState = _procState;
  _plugIn.sampleRate = _sampleRate;
  _plugIn.voiceManager = _voiceManager.get();
  _plugIn.noteMatrix.SetKey(60.0f);
  _plug->AllocOnDemandBuffers(_topo, _procState);
}

void
FBHostProcessor::UpdateNoteMatrix(FBNoteEvent const& event)
{
  // todo smooth versions of all
  bool anyNoteOn = false;
  _noteVelo[event.note.keyUntuned] = 0.0f;
  _noteOn[event.note.keyUntuned] = event.on;
  if (event.on)
  {
    _noteVelo[event.note.keyUntuned] = event.velo;
    _noteMatrix.entries[(int)FBNoteMatrixEntry::LastVeloRaw] = event.velo;
    _noteMatrix.entries[(int)FBNoteMatrixEntry::LastKeyUntunedRaw] = (float)event.note.keyUntuned;
  }
  _noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloVeloRaw] = 1.0f;
  _noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloVeloRaw] = 0.0f;
  _noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyKeyUntunedRaw] = 127.0f;
  _noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyKeyUntunedRaw] = 0.0f;
  for (int i = 0; i < 128; i++)
    if (_noteOn[i])
    {
      anyNoteOn = true;
      if (i < _noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyKeyUntunedRaw])
      {
        _noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyKeyUntunedRaw] = (float)i;
        _noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyVeloRaw] = _noteVelo[i];
      }
      if (i > _noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyKeyUntunedRaw])
      {
        _noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyKeyUntunedRaw] = (float)i;
        _noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyVeloRaw] = _noteVelo[i];
      }
      if (_noteVelo[i] < _noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloVeloRaw])
      {
        _noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloKeyUntunedRaw] = (float)i;
        _noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloVeloRaw] = _noteVelo[i];
      }
      if (_noteVelo[i] > _noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloVeloRaw])
      {
        _noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloKeyUntunedRaw] = (float)i;
        _noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloVeloRaw] = _noteVelo[i];
      }
    }
  if (!anyNoteOn)
  {
    _noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyVeloRaw] = _noteMatrix.entries[(int)FBNoteMatrixEntry::LastVeloRaw];
    _noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyKeyUntunedRaw] = (float)_noteMatrix.entries[(int)FBNoteMatrixEntry::LastKeyUntunedRaw];
    _noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyVeloRaw] = _noteMatrix.entries[(int)FBNoteMatrixEntry::LastVeloRaw];
    _noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyKeyUntunedRaw] = (float)_noteMatrix.entries[(int)FBNoteMatrixEntry::LastKeyUntunedRaw];
    _noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloVeloRaw] = _noteMatrix.entries[(int)FBNoteMatrixEntry::LastVeloRaw];
    _noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloKeyUntunedRaw] = (float)_noteMatrix.entries[(int)FBNoteMatrixEntry::LastKeyUntunedRaw];
    _noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloVeloRaw] = _noteMatrix.entries[(int)FBNoteMatrixEntry::LastVeloRaw];
    _noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloKeyUntunedRaw] = (float)_noteMatrix.entries[(int)FBNoteMatrixEntry::LastKeyUntunedRaw];
  }
}

void 
FBHostProcessor::ProcessHost(
  FBHostInputBlock const& input, FBHostOutputBlock& output)
{
  auto processBeginTime = std::chrono::high_resolution_clock::now();

  _plugIn.bpm = input.bpm;
  _plugIn.prevRoundCpuUsage = _prevRoundCpuUsage;
  _plugIn.projectTimeSamples = input.projectTimeSamples;

  auto denormalState = FBDisableDenormal(); 
  for (auto const& be : input.blockAuto)
    _procState->Params()[be.param].Value(be.normalized);

  // Must be AFTER setting block automation value to parameter state!
  _plug->AllocOnDemandBuffers(_topo, _procState);

  auto const& hostSmoothTimeSpecial = _procState->Special().hostSmoothTime;
  auto const& hostSmoothTimeTopo = hostSmoothTimeSpecial.ParamTopo(*_topo->static_);
  int hostSmoothSamples = hostSmoothTimeTopo.Linear().NormalizedTimeToSamplesFast(hostSmoothTimeSpecial.state->Value(), _sampleRate);
  _procState->SetSmoothingCoeffs(hostSmoothSamples);

  for (int m = 0; m < _topo->modules.size(); m++)
  {
    auto const& indices = _topo->modules[m].topoIndices;
    auto const& static_ = _topo->static_->modules[indices.index];
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
    
    // update key matrix
    int n1 = 0;
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      for (; n1 < _plugIn.noteEvents->size() && (*_plugIn.noteEvents)[n1].pos == s; n1++)
        UpdateNoteMatrix((*_plugIn.noteEvents)[n1]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::LastVeloRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::LastVeloRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::LastKeyUntunedRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::LastKeyUntunedRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloVeloRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloVeloRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloKeyUntunedRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::LowVeloKeyUntunedRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloVeloRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloVeloRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloKeyUntunedRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::HighVeloKeyUntunedRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyVeloRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyVeloRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyKeyUntunedRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::LowKeyKeyUntunedRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyVeloRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyVeloRaw]);
      _plugIn.noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyKeyUntunedRaw].Set(s, _noteMatrix.entries[(int)FBNoteMatrixEntry::HighKeyKeyUntunedRaw]);
    }

    // release old voices
    std::array<bool, FBMaxVoices> seenVoiceOff = {};
    std::array<bool, FBMaxVoices> voiceOffPositions = {};
    for (int n2 = 0; n2 < _plugIn.noteEvents->size(); n2++)
      if (!(*_plugIn.noteEvents)[n2].on)
        for (int v = 0; v < FBMaxVoices; v++)
          if (_plugIn.voiceManager->IsActive(v))
            if ((*_plugIn.noteEvents)[n2].note.Matches(_plugIn.voiceManager->Voices()[v].event.note))
            {
              seenVoiceOff[v] = true;
              voiceOffPositions[v] = (*_plugIn.noteEvents)[n2].pos;
            }

    // aquire new voices, set voice offsetInBlock to possibly nonzero.
    _plug->LeaseVoices(_plugIn);
    _smoothing->ProcessSmoothing(*fixedIn, _plugOut, hostSmoothSamples);
    _plug->ProcessPreVoice(_plugIn);

    // process active voices including newly acquired AND newly released
    for (int v = 0; v < FBMaxVoices; v++)
      if (_plugIn.voiceManager->IsActive(v))
        _plug->ProcessVoice(_plugIn, v, seenVoiceOff[v] ? voiceOffPositions[v] : -1);

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
  FBNoteMatrixInitScalarFromArrayLast(_exchangeState->Host()->noteMatrix, _plugIn.noteMatrix);

  for (int v = 0; v < FBMaxVoices; v++)
    _exchangeState->Voices()[v] = _voiceManager->Voices()[v];

  for (int i = 0; i < _procState->Params().size(); i++)
    if (!_procState->Params()[i].IsAcc())
    {
      if (!_procState->Params()[i].IsVoice())
        *_exchangeState->Params()[i].Global() =
        _procState->Params()[i].GlobalBlock().Value();
      else
        for (int v = 0; v < FBMaxVoices; v++)
          if (_voiceManager->IsActive(v))
            _exchangeState->Params()[i].Voice()[v] =
            _procState->Params()[i].VoiceBlock().Voice()[v];
    }

  FBRestoreDenormal(denormalState);

  auto processEndTime = std::chrono::high_resolution_clock::now();
  auto processDuration = std::chrono::duration_cast<std::chrono::microseconds>(processEndTime - processBeginTime);
  float durationSeconds = processDuration.count() / (1000.0f * 1000.0f);
  float windowTimeSeconds = input.sampleCount / _sampleRate;
  _prevRoundCpuUsage = durationSeconds / windowTimeSeconds;
}