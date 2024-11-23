#include <playground_plug/base/FFOutputSplitter.hpp>
#include <algorithm>

FFOutputSplitter::
FFOutputSplitter(int maxHostSampleCount):
_accumulating(std::max(FF_FIXED_BLOCK_SIZE, maxHostSampleCount)) {}

FFAccumulatingOutputBlock const&
FFOutputSplitter::GetAccumulatedBlock() const
{
  return _accumulating;
}

void 
FFOutputSplitter::RemoveSamples(int count)
{
  _accumulating.sampleCount -= count;
  _accumulating.audio.ShiftLeft(count);
}

void 
FFOutputSplitter::AccumulateFixedBlock(
  FFFixedStereoBlock const& audioOut)
{
  // todo accumulating->accumulated + copyto
  for (int c = 0; c < FF_CHANNELS_STEREO; c++)
    for (int s = 0; s < FF_FIXED_BLOCK_SIZE; s++)
    {
      float sample = audioOut[c][s];
      int accumulatedPos = _accumulating.sampleCount + s;
      _accumulating.audio[c][accumulatedPos] = sample;
    }
  _accumulating.sampleCount += FF_FIXED_BLOCK_SIZE;
}