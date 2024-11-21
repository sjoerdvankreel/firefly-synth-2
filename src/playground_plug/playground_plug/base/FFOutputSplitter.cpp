#include <playground_plug/base/FFOutputSplitter.hpp>
#include <algorithm>

FFOutputSplitter::
FFOutputSplitter(int maxHostSampleCount):
_accumulatedBlock(std::max(FF_BLOCK_SIZE, maxHostSampleCount)) {}

FFHostOutputBlock const& 
FFOutputSplitter::GetAccumulatedBlock() const
{
  return _accumulatedBlock;
}

void 
FFOutputSplitter::RemoveSamples(int count)
{
  _accumulatedBlock.sampleCount -= count;
  _accumulatedBlock.audio.ShiftLeft(count);
}

void 
FFOutputSplitter::AccumulateFixedBlock(
  FFFixedStereoBlock const& audioOut)
{
  for (int channel = 0; channel < FF_CHANNELS_STEREO; channel++)
    for (int sample = 0; sample < FF_BLOCK_SIZE; sample++)
    {
      float blockSample = audioOut[channel][sample];
      int accumulatedPos = _accumulatedBlock.sampleCount + sample;
      _accumulatedBlock.audio[channel][accumulatedPos] = blockSample;
    }
  _accumulatedBlock.sampleCount += FF_BLOCK_SIZE;
}