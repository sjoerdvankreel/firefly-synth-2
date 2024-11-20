#include <playground_plug/base/dsp/FBHostBlock.hpp>

FBHostBlock::
FBHostBlock(int maxSampleCount)
{
  noteEvents.reserve(FB_EVENT_COUNT_GUESS);
  blockParamEvents.reserve(FB_EVENT_COUNT_GUESS);
  accurateParamEvents.reserve(FB_EVENT_COUNT_GUESS);
  for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
  {
    audioIn[channel].reserve(maxSampleCount);
    audioOut[channel].reserve(maxSampleCount);
  }
}