#include <playground_plug/base/dsp/FBHostBlock.hpp>

FBHostBlock::
FBHostBlock(int maxSampleCount)
{
  int const eventCountGuess = 1024;
  noteEvents.reserve(eventCountGuess);
  autoEvents.reserve(eventCountGuess);
  plugEvents.reserve(eventCountGuess);
  for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
  {
    audioIn[channel].reserve(maxSampleCount);
    audioOut[channel].reserve(maxSampleCount);
  }
}