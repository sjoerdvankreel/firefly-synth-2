#include <playground_plug_vst3/FFVST3PluginProcessor.hpp>

tresult PLUGIN_API 
FFVST3PluginProcessor::process(ProcessData& data)
{
  if (data.numOutputs != 2) return kResultTrue;
  _processor.Process(nullptr, data.outputs->channelBuffers32, data.numSamples);
  return kResultTrue;
}