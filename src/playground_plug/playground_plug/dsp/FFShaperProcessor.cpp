#if 0

#include <playground_plug/plug/dsp/FFShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <cmath>

void 
FFShaperProcessor::Process(FBProcessorContext const& context, FFProcessorMemory& memory)
{
  for(int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
    for (int s = 0; s < FF_BLOCK_SIZE; s++)
      memory.shaperOut[context.moduleSlot][channel][s] = std::tanh(memory.shaperIn[context.moduleSlot][channel][s]);
}

#endif