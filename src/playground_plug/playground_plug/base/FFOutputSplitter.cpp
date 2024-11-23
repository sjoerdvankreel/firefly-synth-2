#include <playground_plug/base/FFOutputSplitter.hpp>
#include <algorithm>

FFOutputSplitter::
FFOutputSplitter(int maxHostSampleCount):
_accumulated(std::max(FF_FIXED_BLOCK_SIZE, maxHostSampleCount)) {}

FFAccumulatedOutputBlock const&
FFOutputSplitter::GetAccumulatedBlock() const
{
  return _accumulated;
}

void 
FFOutputSplitter::RemoveSamples(int count)
{
  _accumulated.sampleCount -= count;
  _accumulated.audio.ShiftLeft(count);
}

void 
FFOutputSplitter::AccumulateFixedBlock(
  FFFixedStereoBlock const& output)
{
  output.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, FF_FIXED_BLOCK_SIZE);
  _accumulated.sampleCount += FF_FIXED_BLOCK_SIZE;
}