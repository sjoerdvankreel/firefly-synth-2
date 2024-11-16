#include <playground_plug/plug/shared/FFPluginTopology.hpp>
#include <playground_plug/plug/dsp/FFWaveShaperProcessor.hpp>
#include <cmath>

void 
FFWaveShaperProcessor::Process(int moduleSlot, FFPluginProcessorBlock& processorBlock)
{
  for (int s = 0; s < FF_BLOCK_SIZE; s++)
  {
    processorBlock.waveShaperAudioOutput[moduleSlot][0][s] = std::tanh(processorBlock.waveShaperAudioInput[moduleSlot][0][s]);
    processorBlock.waveShaperAudioOutput[moduleSlot][1][s] = std::tanh(processorBlock.waveShaperAudioInput[moduleSlot][0][s]);
  }
}