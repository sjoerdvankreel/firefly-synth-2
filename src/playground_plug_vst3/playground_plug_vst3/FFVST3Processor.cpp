#include <playground_plug_vst3/FFVST3Processor.hpp>

FFVST3Processor::
FFVST3Processor(FUID const& controllerId)
{
  setControllerClass(controllerId);
}

tresult PLUGIN_API
FFVST3Processor::setupProcessing(ProcessSetup& setup)
{
  _processor.reset(new FFProcessor(setup.sampleRate));
  return kResultTrue;
}

tresult PLUGIN_API
FFVST3Processor::initialize(FUnknown* context)
{
  if (AudioEffect::initialize(context) != kResultTrue)
    return kResultFalse;
  addEventInput(STR16("Event In"));
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  return kResultTrue;
}

tresult PLUGIN_API
FFVST3Processor::canProcessSampleSize(int32 symbolicSize)
{
  if (symbolicSize == kSample32) 
    return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API
FFVST3Processor::process(ProcessData& data)
{
  if (data.numOutputs != 1 || data.outputs[0].numChannels != 2)
    return kResultTrue;
  _processor->Process(nullptr, data.outputs->channelBuffers32, data.numSamples);
  return kResultTrue;
}

tresult PLUGIN_API
FFVST3Processor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
  if (numIns != 0 || numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}