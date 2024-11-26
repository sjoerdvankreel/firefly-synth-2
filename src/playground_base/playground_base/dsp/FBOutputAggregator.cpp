#include <playground_base/dsp/FBOutputAggregator.hpp>

FBOutputAggregator::
FBOutputAggregator(int maxHostSampleCount):
_maxHostSampleCount(maxHostSampleCount),
_hitMaxHostSampleCount(false),
_accumulated(2 * (FB_FIXED_BLOCK_SIZE + maxHostSampleCount)) {}

void 
FBOutputAggregator::Accumulate(FBFixedStereoBlock const& input)
{

  for (int zz = 0; zz < _accumulated.sampleCount - 1; zz++)
  {
    if (std::abs(_accumulated.audio[0][zz] - _accumulated.audio[0][zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }

  input.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, FB_FIXED_BLOCK_SIZE);
  _accumulated.sampleCount += FB_FIXED_BLOCK_SIZE;

  for (int zz = 0; zz < _accumulated.sampleCount - 1; zz++)
  {
    if (std::abs(_accumulated.audio[0][zz] - _accumulated.audio[0][zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }
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

  for (int zz = 0; zz < _accumulated.sampleCount - 1; zz++)
  {
    if (std::abs(_accumulated.audio[0][zz] - _accumulated.audio[0][zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }

  int samplesUsed = std::min(output.Count(), _accumulated.sampleCount);
  assert(samplesUsed == output.Count());
  _accumulated.audio.CopyTo(output, 0, 0, samplesUsed);
  _accumulated.audio.ShiftLeft(samplesUsed);
  _accumulated.sampleCount -= samplesUsed;

  for (int zz = 0; zz < _accumulated.sampleCount - 1; zz++)
  {
    if (std::abs(_accumulated.audio[0][zz] - _accumulated.audio[0][zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }
}