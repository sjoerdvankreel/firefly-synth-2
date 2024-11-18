#include <playground_plug/base/dsp/FBHostBlock.hpp>

FBHostBlock::
FBHostBlock(int maxSampleCount)
{
  for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
  {
    audioIn[channel].reserve(maxSampleCount);
    audioOut[channel].reserve(maxSampleCount);
  }
}