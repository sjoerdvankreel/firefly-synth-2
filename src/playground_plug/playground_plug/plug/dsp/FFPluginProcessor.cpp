#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>

FFPluginProcessor::
FFPluginProcessor(float sampleRate) :
_sampleRate(sampleRate)
{
}

void 
FFPluginProcessor::Process(FB_RAW_INPUT_BUFFER in, FB_RAW_OUTPUT_BUFFER out, std::size_t sampleCount)
{
}