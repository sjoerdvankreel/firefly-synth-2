#include <playground_base_vst3/FBVST3AudioEffect.hpp>
#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <playground_base/dsp/host/FBHostProcessor.hpp>

#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>

#include <map>
#include <algorithm>

static FBBlockEvent
MakeBlockEvent(int index, ParamValue value)
{
  FBBlockEvent result;
  result.index = index;
  result.normalized = value;
  return result;
}

static FBAccEvent
MakeAccEvent(int index, int pos, ParamValue value)
{
  FBAccEvent result;
  result.pos = pos;
  result.index = index;
  result.normalized = value;
  return result;
}

static FBNoteEvent
MakeNoteOnEvent(Event const& event)
{
  FBNoteEvent result;
  result.on = true;
  result.pos = event.sampleOffset;
  result.velo = event.noteOn.velocity;
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
  result.pos = event.sampleOffset;
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
FBVST3AudioEffect(
  FBStaticTopo const& topo, FUID const& controllerId):
_topo(std::make_unique<FBRuntimeTopo>(topo))
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
FBVST3AudioEffect::setBusArrangements(
  SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
  if (numIns != 0 || numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}

tresult PLUGIN_API
FBVST3AudioEffect::setupProcessing(ProcessSetup& setup)
{
  for (int ch = 0; ch < 2; ch++)
    _zeroIn[ch] = std::vector<float>(setup.maxSamplesPerBlock, 0.0f);
  _hostProcessor.reset(new FBHostProcessor(MakePlugProcessor(*_topo, setup.sampleRate)));
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3AudioEffect::process(ProcessData& data)
{
  Event event;
  _input.note.clear();
  if (data.inputEvents != nullptr)
    for (int i = 0; i < data.inputEvents->getEventCount(); i++)
      if (data.inputEvents->getEvent(i, event) == kResultOk)
        if (event.type == Event::kNoteOnEvent)
          _input.note.push_back(MakeNoteOnEvent(event));
        else if (event.type == Event::kNoteOffEvent)
          _input.note.push_back(MakeNoteOffEvent(event));

  int position;
  ParamValue value;
  IParamValueQueue* queue;
  std::map<int, int>::const_iterator iter;
  _input.acc.clear();
  _input.block.clear();
  if(data.inputParameterChanges != nullptr)
    for (int p = 0; p < data.inputParameterChanges->getParameterCount(); p++)
      if ((queue = data.inputParameterChanges->getParameterData(p)) != nullptr)
        if(queue->getPointCount() > 0)
          if ((iter = _topo->tagToBlock.find(queue->getParameterId())) != _topo->tagToBlock.end())
          {
            if (queue->getPoint(queue->getPointCount() - 1, position, value) == kResultTrue)
              _input.block.push_back(MakeBlockEvent(iter->second, value));
          } else if ((iter = _topo->tagToAcc.find(queue->getParameterId())) != _topo->tagToAcc.end())
          {
            for (int point = 0; point < queue->getPointCount(); point++)
              if (queue->getPoint(point, position, value) == kResultTrue)
                _input.acc.push_back(MakeAccEvent(iter->second, position, value));
          }     

  auto compare = [](auto& l, auto& r) {
    return l.index == r.index ? l.pos < r.pos : l.index < r.index; };
  std::sort(_input.acc.begin(), _input.acc.end(), compare);

  if (data.numInputs == 1)
    _input.audio = MakeHostAudioBlock(data.inputs[0], data.numSamples);
  else
    _input.audio = FBHostAudioBlock(_zeroIn[0].data(), _zeroIn[1].data(), data.numSamples);
  FBHostAudioBlock output(MakeHostAudioBlock(*data.outputs, data.numSamples));
  _hostProcessor->ProcessHost(_input, output);
  return kResultTrue;
}