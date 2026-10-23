#include <firefly_base_vst3/FBVST3Plugin.hpp>
#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base_vst3/FBVST3GUIEditor.hpp>

#include <firefly_base/dsp/host/FBHostProcessor.hpp>
#include <firefly_base/dsp/plug/FBPlugProcessor.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/shared/FBExchangeStateQueue.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGUIStateContainer.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>

#include <cstring>
#include <algorithm>
#include <unordered_map>

static FBBlockAutoEvent
MakeBlockAutoEvent(int param, ParamValue value)
{
  FBBlockAutoEvent result = {};
  result.param = param;
  result.normalized = value;
  return result;
}

static FBAccAutoEvent
MakeAccAutoEvent(int param, int pos, ParamValue value)
{
  FBAccAutoEvent result = {};
  result.pos = pos;
  result.param = param;
  result.value = value;
  return result;
}

static FBNoteEvent
MakeNoteOnEvent(Event const& event)
{
  FBNoteEvent result = {};
  result.on = true;
  result.pos = event.sampleOffset;
  result.velo = event.noteOn.velocity;
  result.note.id = event.noteOn.noteId;
  result.note.channel = event.noteOn.channel;
  result.note.key = event.noteOn.pitch;
  return result;
}

static FBNoteEvent
MakeNoteOffEvent(Event const& event)
{
  FBNoteEvent result = {};
  result.on = false;
  result.pos = event.sampleOffset;
  result.velo = event.noteOff.velocity;
  result.note.id = event.noteOff.noteId;
  result.note.channel = event.noteOff.channel;
  result.note.key = event.noteOff.pitch;
  return result;
}

static FBMIDIEvent
MakeMIDIEvent(int eventId, int pos, ParamValue value)
{
  FBMIDIEvent result = {};
  result.pos = pos;
  result.value = value;
  if (0 <= eventId && eventId < FBMIDIEvent::CCMessageCount)
  {
    result.controlChange = eventId;
    result.message = FBMIDIEvent::CCMessageId;
    return result;
  }
  result.controlChange = 0;
  result.message = eventId;
  return result;
}

tresult PLUGIN_API
FBVST3Plugin::canProcessSampleSize(int32 symbolicSize)
{
  if (symbolicSize == kSample32)
    return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API
FBVST3Plugin::getState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, state]()
  {
    FBScalarStateContainer audioParams(*Topo());
    audioParams.CopyFrom(this, false);
    std::string json = Topo()->SaveStateToString(*this, audioParams, false);
    if (!FBVST3SaveIBStream(state, json))
      return kResultFalse;
    return kResultOk;
  });
}

tresult PLUGIN_API
FBVST3Plugin::setState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, state]()
  {
    std::string json;
    if (!FBVST3LoadIBStream(state, json))
      return kResultFalse;
    FBScalarStateContainer audioParams(*Topo());
    if (!Topo()->LoadStateFromString(json, *this, audioParams, false))
      return kResultTrue; // not crash bitwig
    audioParams.CopyTo(this, false);
    
    OnPatchNameChanged();
    OnInstanceNameChanged();
    OnGUIAudioClipsChanged();
    MarkPatchAsSessionState();
    NotifyHostOfParamNameChanges();
    if (_guiEditor != nullptr)
      for (int i = 0; i < GUIState()->Params().size(); i++)
        _guiEditor->SetGUIParamNormalizedFromHost(i, GetGUIParamNormalized(i));
    return kResultOk;
  });
}

tresult PLUGIN_API
FBVST3Plugin::setupProcessing(ProcessSetup& setup)
{
  FB_LOG_ENTRY_EXIT();
  return FBWithLogException([this, &setup]() 
  {
    _sampleRate = static_cast<float>(setup.sampleRate);
    for (int ch = 0; ch < 2; ch++)
      _zeroIn[ch] = std::vector<float>(setup.maxSamplesPerBlock, 0.0f);
    _hostProcessor.reset(new FBHostProcessor(this));
    return kResultTrue;
  });
}

tresult PLUGIN_API
FBVST3Plugin::setBusArrangements(
  SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
  if (Topo()->static_->meta.isFx)
  {
    if (numIns != 0 && numIns != 1 && numIns != 2)
      return kResultFalse;
    if (numIns > 0 && inputs[0] != SpeakerArr::kStereo)
      return kResultFalse;
    if(numIns > 1 && inputs[1] != SpeakerArr::kStereo)
      return kResultFalse;
  }
  else
  {
    if (numIns != 0 && numIns != 1)
      return kResultFalse;
    if (numIns > 0 && inputs[0] != SpeakerArr::kStereo)
      return kResultFalse;
  } 
  return SingleComponentEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}

void 
FBVST3Plugin::InitializeAudioEffect()
{
  FB_LOG_ENTRY_EXIT();
  FBWithLogException([this]()
  {
    addEventInput(STR16("Event In"));
    addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);

    // The bustype is important to bitwig.
    // With main, it wont accept sidechaining.
    // We allow side in for synth and fx both.
    if (Topo()->static_->meta.isFx)
      addAudioInput(STR16("Stereo In"), SpeakerArr::kStereo, BusTypes::kMain);
    addAudioInput(STR16("Sidechain"), SpeakerArr::kStereo, BusTypes::kAux);
  });
}

tresult PLUGIN_API
FBVST3Plugin::process(ProcessData& data)
{
  return FBWithLogException([this, &data]()
  {
    _input.projectTimeSamples = 0;
    _input.sampleCount = data.numSamples;
    _input.bpm = FBHostInputBlock::DefaultBPM;
    if (data.processContext != nullptr)
    {
      if ((data.processContext->state & ProcessContext::kTempoValid) != 0)
        _input.bpm = static_cast<float>(data.processContext->tempo);
      _input.projectTimeSamples = static_cast<float>(data.processContext->projectTimeSamples);
    }

    Event inEvent;
    _input.noteEvents.clear();
    if (data.inputEvents != nullptr)
      for (int i = 0; i < data.inputEvents->getEventCount(); i++)
        if (data.inputEvents->getEvent(i, inEvent) == kResultOk)
          if (inEvent.type == Event::kNoteOnEvent)
            _input.noteEvents.push_back(MakeNoteOnEvent(inEvent));
          else if (inEvent.type == Event::kNoteOffEvent)
            _input.noteEvents.push_back(MakeNoteOffEvent(inEvent));

    int position;
    ParamValue value;
    IParamValueQueue* inQueue;
    std::unordered_map<int, int>::const_iterator iter;
    _input.blockAuto.clear();
    auto& accAuto = _input.accAutoByParamThenSample;
    accAuto.clear();
    auto& midi = _input.midiByMessageThenCCThenSample;
    midi.clear();
    if (data.inputParameterChanges != nullptr)
      for (int p = 0; p < data.inputParameterChanges->getParameterCount(); p++)
        if ((inQueue = data.inputParameterChanges->getParameterData(p)) != nullptr)
          if (inQueue->getPointCount() > 0)
          {
            int paramId = inQueue->getParameterId();
            if (paramId >= FBVST3MIDIParameterMappingBegin && paramId < FBVST3MIDIParameterMappingBegin + FBMIDIEvent::MessageCount)
            {
              for (int point = 0; point < inQueue->getPointCount(); point++)
                if (inQueue->getPoint(point, position, value) == kResultTrue)
                  midi.push_back(MakeMIDIEvent(paramId - FBVST3MIDIParameterMappingBegin, position, value));
            }
            else if ((iter = Topo()->audio.paramTagToIndex.find(paramId)) != Topo()->audio.paramTagToIndex.end())
            {
              if (Topo()->audio.params[iter->second].static_.mode == FBParamMode::Accurate ||
                Topo()->audio.params[iter->second].static_.mode == FBParamMode::VoiceStart)
              {
                for (int point = 0; point < inQueue->getPointCount(); point++)
                  if (inQueue->getPoint(point, position, value) == kResultTrue)
                    accAuto.push_back(MakeAccAutoEvent(iter->second, position, value));
              }
              else
              {
                if (inQueue->getPoint(inQueue->getPointCount() - 1, position, value) == kResultTrue)
                  _input.blockAuto.push_back(MakeBlockAutoEvent(iter->second, value));
              }
            }
          }
    
    std::sort(midi.begin(), midi.end(), FBMIDIEventOrderByMessageThenCCThenPos);
    std::sort(accAuto.begin(), accAuto.end(), FBAccAutoEventOrderByParamThenPos);

    _output.outputParams.clear();
    _output.audio = FBHostAudioBlock(data.outputs->channelBuffers32, data.numSamples);

    float* zeroIn[2] = { _zeroIn[0].data(), _zeroIn[1].data() };
    _input.mainAudio = FBHostAudioBlock(zeroIn, data.numSamples);
    _input.sidechainAudio = FBHostAudioBlock(zeroIn, data.numSamples);
    if (!Topo()->static_->meta.isFx)
    {
      if(data.numInputs > 0 && data.inputs[0].numChannels == 2 && data.inputs[0].channelBuffers32 != nullptr)
        _input.sidechainAudio = FBHostAudioBlock(data.inputs[0].channelBuffers32, data.numSamples);
    }
    else
    {
      if (data.numInputs > 0 && data.inputs[0].numChannels == 2 && data.inputs[0].channelBuffers32 != nullptr)
        _input.mainAudio = FBHostAudioBlock(data.inputs[0].channelBuffers32, data.numSamples);
      if (data.numInputs > 1 && data.inputs[1].numChannels == 2 && data.inputs[1].channelBuffers32 != nullptr)
        _input.sidechainAudio = FBHostAudioBlock(data.inputs[1].channelBuffers32, data.numSamples);
    }

    _hostProcessor->ProcessHost(_input, _output);
    ExchangeStateQueue()->TryEnqueue(ExchangeToGUIState()->Raw());

    if (data.outputParameterChanges != nullptr)
      for (int i = 0; i < _output.outputParams.size(); i++)
      {
        int unused;
        auto const& outEvent = _output.outputParams[i];
        int tag = Topo()->audio.params[outEvent.param].tag;
        auto outQueue = data.outputParameterChanges->addParameterData(tag, unused);
        if (outQueue != nullptr)
          outQueue->addPoint(0, outEvent.normalized, unused);
      }

    return kResultTrue;
  });
}