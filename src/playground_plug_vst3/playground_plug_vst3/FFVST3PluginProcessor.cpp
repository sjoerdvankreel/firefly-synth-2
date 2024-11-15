#include <playground_plug_vst3/FFVST3PluginProcessor.hpp>

FFVST3PluginProcessor::
FFVST3PluginProcessor(FUID const& controllerId)
{
  setControllerClass(controllerId);
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
FFVST3PluginProcessor::process(ProcessData& data)
{
  if (data.numOutputs != 2) 
    return kResultTrue;
  _processor.Process(nullptr, data.outputs->channelBuffers32, data.numSamples);
  return kResultTrue;
}

tresult PLUGIN_API
FFVST3PluginProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
  if (numIns != 0 || numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}