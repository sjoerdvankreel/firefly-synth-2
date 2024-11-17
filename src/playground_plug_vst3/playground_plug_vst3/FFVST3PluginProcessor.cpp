#include <playground_plug_vst3/FFVST3PluginProcessor.hpp>
#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <algorithm>

static FBPlugEvent
MakePlugEvent(int tag, ParamValue value)
{
  FBPlugEvent result;
  result.tag = tag;
  result.normalized = value;
  return result;
}

static FBAutoEvent
MakeAutoEvent(int tag, int position, ParamValue value)
{
  FBAutoEvent result;
  result.tag = tag;
  result.normalized = value;
  result.position = position;
  return result;
}

static FBNoteEvent
MakeNoteOnEvent(Event const& event)
{
  FBNoteEvent result;
  result.on = true;
  result.id = event.noteOn.noteId;
  result.key = event.noteOn.pitch;
  result.velo = event.noteOn.velocity;
  result.channel = event.noteOn.channel;
  result.position = event.sampleOffset;
  return result;
}

static FBNoteEvent
MakeNoteOffEvent(Event const& event)
{
  FBNoteEvent result;
  result.on = false;
  result.id = event.noteOff.noteId;
  result.key = event.noteOff.pitch;
  result.velo = event.noteOff.velocity;
  result.channel = event.noteOff.channel;
  result.position = event.sampleOffset;
  return result;
}

FFVST3PluginProcessor::
FFVST3PluginProcessor(FUID const& controllerId):
_topo(FFMakeTopo())
{
  setControllerClass(controllerId);
}

tresult PLUGIN_API
FFVST3PluginProcessor::setupProcessing(ProcessSetup& setup)
{
  _processor.reset(new FFPluginProcessor(setup.maxSamplesPerBlock, setup.sampleRate));
  return kResultTrue;
}

tresult PLUGIN_API
FFVST3PluginProcessor::initialize(FUnknown* context)
{
  if (AudioEffect::initialize(context) != kResultTrue)
    return kResultFalse;
  addEventInput(STR16("Event In"));
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  return kResultTrue;
}

tresult PLUGIN_API
FFVST3PluginProcessor::canProcessSampleSize(int32 symbolicSize)
{
  if (symbolicSize == kSample32) 
    return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API
FFVST3PluginProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
  if (numIns != 0 || numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}

tresult PLUGIN_API
FFVST3PluginProcessor::process(ProcessData& data)
{
  if (data.numOutputs != 1 || data.outputs[0].numChannels != 2)
    return kResultTrue;
  
  _hostBlock.audioIn = nullptr;
  _hostBlock.sampleCount = data.numSamples;
  _hostBlock.audioOut = data.outputs[0].channelBuffers32;

  Event event;
  _hostBlock.noteEvents.clear();
  if (data.inputEvents != nullptr)
    for (int i = 0; i < data.inputEvents->getEventCount(); i++)
      if (data.inputEvents->getEvent(i, event) == kResultOk)
        if (event.type == Event::kNoteOnEvent)
          _hostBlock.noteEvents.push_back(MakeNoteOnEvent(event));
        else if (event.type == Event::kNoteOffEvent)
          _hostBlock.noteEvents.push_back(MakeNoteOffEvent(event));

  int position;
  ParamValue value;
  IParamValueQueue* queue;
  std::map<int, int>::const_iterator iter;
  _hostBlock.plugEvents.clear();
  _hostBlock.autoEvents.clear();
  if(data.inputParameterChanges != nullptr)
    for (int param = 0; param < data.inputParameterChanges->getParameterCount(); param++)
      if ((queue = data.inputParameterChanges->getParameterData(param)) != nullptr)
        if ((iter = _topo.tagToPlugParam.find(queue->getParameterId())) != _topo.tagToPlugParam.end())
        {
          if (queue->getPoint(queue->getPointCount() - 1, position, value) == kResultTrue)
            _hostBlock.plugEvents.push_back(MakePlugEvent(queue->getParameterId(), value));
        }
        else if ((iter = _topo.tagToAutoParam.find(queue->getParameterId())) != _topo.tagToAutoParam.end())
        {
          for (int point = 0; point < queue->getPointCount(); point++)
            if (queue->getPoint(point, position, value) == kResultTrue)
              _hostBlock.autoEvents.push_back(MakeAutoEvent(queue->getParameterId(), position, value));
        }        

  // sort by position
  // CLAP also does it this way 
  // and it makes the block splitting easier
  auto compare = [](auto const& l, auto const& r) { return l.position < r.position; };
  std::sort(_hostBlock.autoEvents.begin(), _hostBlock.autoEvents.end(), compare);

  _processor->ProcessHostBlock(_hostBlock);
  return kResultTrue;
}