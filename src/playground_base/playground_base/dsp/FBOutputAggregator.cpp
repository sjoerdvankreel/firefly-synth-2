#include <playground_base/dsp/FBOutputAggregator.hpp>

FBOutputAggregator::
FBOutputAggregator(int maxHostSampleCount):
_maxHostSampleCount(maxHostSampleCount),
_hitMaxHostSampleCount(false),
_accumulated(2 * (FB_FIXED_BLOCK_SIZE + maxHostSampleCount)) {}

void 
FBOutputAggregator::Accumulate(FBFixedStereoBlock const& input)
{
  input.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, FB_FIXED_BLOCK_SIZE);
  _accumulated.sampleCount += FB_FIXED_BLOCK_SIZE;
}

void 
FBOutputAggregator::Aggregate(FBRawStereoBlockView& output)
{
  if (_accumulated.sampleCount >= _maxHostSampleCount)
    _hitMaxHostSampleCount = true;
  if (!_hitMaxHostSampleCount)
  {
    // TODO PDC
    output.Fill(0, output.Count(), 0.0f);
    return;
  }

  int samplesUsed = std::min(output.Count(), _accumulated.sampleCount);
  _accumulated.audio.CopyTo(output, 0, 0, samplesUsed);
  _accumulated.audio.ShiftLeft(samplesUsed);
  _accumulated.sampleCount -= samplesUsed;
}