#include <playground_plug/base/dsp/FBHostBlock.hpp>

FBHostBlock::
FBHostBlock(int maxHostSampleCount)
{
  audioIn[FB_CHANNEL_L].reserve(maxHostSampleCount);
  audioIn[FB_CHANNEL_R].reserve(maxHostSampleCount);
  audioOut[FB_CHANNEL_L].reserve(maxHostSampleCount);
  audioOut[FB_CHANNEL_R].reserve(maxHostSampleCount);
}