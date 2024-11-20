#include <playground_plug/dsp/FFHostBlock.hpp>

FFHostBlock::
FFHostBlock(int maxSampleCount)
{
  noteEvents.reserve(FF_EVENT_COUNT_GUESS);
  blockParamEvents.reserve(FF_EVENT_COUNT_GUESS);
  accurateParamEvents.reserve(FF_EVENT_COUNT_GUESS);
  for (int channel = 0; channel < FF_CHANNELS_STEREO; channel++)
  {
    audioIn[channel].reserve(maxSampleCount);
    audioOut[channel].reserve(maxSampleCount);
  }
}