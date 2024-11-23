#include <playground_plug/base/dsp/FFOutputAggregator.hpp>
#include <algorithm>

FFOutputAggregator::
FFOutputAggregator(int maxHostSampleCount):
_accumulated(std::max(FF_FIXED_BLOCK_SIZE, maxHostSampleCount)) {}

void 
FFOutputAggregator::Accumulate(
  FFFixedStereoBlock const& input)
{
  input.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, FF_FIXED_BLOCK_SIZE);
  _accumulated.sampleCount += FF_FIXED_BLOCK_SIZE;
}

void 
FFOutputAggregator::Aggregate(FFRawStereoBlock& output)
{
  int samplesUsed = std::min(output.Count(), _accumulated.sampleCount);
  int samplesPadded = std::max(0, output.Count() - _accumulated.sampleCount);
  
  output.Fill(0, samplesPadded, 0.0f);
  _accumulated.audio.CopyTo(output, 0, samplesPadded, samplesUsed);
  _accumulated.audio.ShiftLeft(samplesUsed);
  _accumulated.sampleCount -= samplesUsed;
}