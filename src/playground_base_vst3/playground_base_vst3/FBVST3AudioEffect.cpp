#include <playground_base_vst3/FBVST3AudioEffect.hpp>
#include <playground_base/dsp/shared/FBNoteEvent.hpp>
#include <playground_base/dsp/host/FBHostAudioBlock.hpp>

#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <algorithm>

static FBBlockParamEvent
MakeBlockParamEvent(int index, ParamValue value)
{
  FBBlockParamEvent result;
  result.index = index;
  result.normalized = value;
  return result;
}

static FBAccParamEvent
MakeAccParamEvent(int index, int position, ParamValue value)
{
  FBAccParamEvent result;
  result.index = index;
  result.normalized = value;
  result.position = position;
  return result;
}

static FBNoteEvent
MakeNoteOnEvent(Event const& event)
{
  FBNoteEvent result;
  result.on = true;
  result.velo = event.noteOn.velocity;
  result.position = event.sampleOffset;
  result.note.id = event.noteOn.noteId;
  result.note.key = event.noteOn.pitch;
  result.note.channel = event.noteOn.channel;
  return result;
}

static FBNoteEvent
MakeNoteOffEvent(Event const& event)
{
  FBNoteEvent result;
  result.on = false;
  result.position = event.sampleOffset;
  result.velo = event.noteOff.velocity;
  result.note.id = event.noteOff.noteId;
  result.note.key = event.noteOff.pitch;
  result.note.channel = event.noteOff.channel;
  return result;
}

static FBHostAudioBlock
MakeHostAudioBlock(AudioBusBuffers& buffers, int sampleCount)
{
  return FBHostAudioBlock(
    buffers.channelBuffers32[0],
    buffers.channelBuffers32[1],
    sampleCount);
}

FBVST3AudioEffect::
FBVST3AudioEffect(FBRuntimeTopo&& topo, FUID const& controllerId):
_topo(std::move(topo))
{
  setControllerClass(controllerId);
}

tresult PLUGIN_API
FBVST3AudioEffect::initialize(FUnknown* context)
{
  if (AudioEffect::initialize(context) != kResultTrue)
    return kResultFalse;
  addEventInput(STR16("Event In"));
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3AudioEffect::canProcessSampleSize(int32 symbolicSize)
{
  if (symbolicSize == kSample32) 
    return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API
FBVST3AudioEffect::setupProcessing(ProcessSetup& setup)
{
  _processor = CreateProcessor(_topo, setup);
  _input.reset(new FBHostInputBlock(nullptr, nullptr, 0));
  for (int ch = 0; ch < 2; ch++)
    _zeroIn[ch] = std::vector<float>(setup.maxSamplesPerBlock, 0.0f);
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3AudioEffect::setBusArrangements(
  SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
  if (numIns != 0 || numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}

tresult PLUGIN_API
FBVST3AudioEffect::process(ProcessData& data)
{
  Event event;
  _input->events.note.clear();
  if (data.inputEvents != nullptr)
    for (int i = 0; i < data.inputEvents->getEventCount(); i++)
      if (data.inputEvents->getEvent(i, event) == kResultOk)
        if (event.type == Event::kNoteOnEvent)
          _input->events.note.push_back(MakeNoteOnEvent(event));
        else if (event.type == Event::kNoteOffEvent)
          _input->events.note.push_back(MakeNoteOffEvent(event));

  int position;
  ParamValue value;
  IParamValueQueue* queue;
  std::map<int, int>::const_iterator iter;
  _input->events.accParam.clear();
  _input->events.blockParam.clear();
  if(data.inputParameterChanges != nullptr)
    for (int p = 0; p < data.inputParameterChanges->getParameterCount(); p++)
      if ((queue = data.inputParameterChanges->getParameterData(p)) != nullptr)
        if(queue->getPointCount() > 0)
          if ((iter = _topo.tagToBlockParam.find(queue->getParameterId())) != _topo.tagToBlockParam.end())
          {
            if (queue->getPoint(queue->getPointCount() - 1, position, value) == kResultTrue)
              _input->events.blockParam.push_back(MakeBlockParamEvent(iter->second, value));
          } else if ((iter = _topo.tagToAccParam.find(queue->getParameterId())) != _topo.tagToAccParam.end())
          {
            for (int point = 0; point < queue->getPointCount(); point++)
              if (queue->getPoint(point, position, value) == kResultTrue)
                _input->events.accParam.push_back(MakeAccParamEvent(iter->second, position, value));
          }     

  auto compare = [](auto& l, auto& r) {
    return l.index == r.index ? l.position < r.position : l.index < r.index; };
  std::sort(_input->events.accParam.begin(), _input->events.accParam.end(), compare);

  if (data.numInputs == 1)
    _input->audio = MakeHostAudioBlock(data.inputs[0], data.numSamples);
  else
    _input->audio = FBHostAudioBlock(
      _zeroIn[0].data(), 
      _zeroIn[1].data(), 
      data.numSamples);

  FBHostAudioBlock output(MakeHostAudioBlock(*data.outputs, data.numSamples));
  _processor->ProcessHost(*_input, output);
  return kResultTrue;
}