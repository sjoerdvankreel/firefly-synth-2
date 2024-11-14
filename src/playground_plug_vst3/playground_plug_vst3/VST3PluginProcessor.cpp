#include <playground_plug/plug.hpp>
#include <playground_plug_vst3/VST3PluginProcessor.hpp>

tresult PLUGIN_API 
VST3PluginProcessor::process(ProcessData& data)
{
  if (data.numOutputs != 2) return kResultTrue;
  process()
  return kResultTrue;
}