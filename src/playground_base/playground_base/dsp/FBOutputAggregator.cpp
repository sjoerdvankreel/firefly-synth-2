#include <playground_base/dsp/FBOutputAggregator.hpp>

FBOutputAggregator::
FBOutputAggregator(int maxHostSampleCount) :
  _maxHostSampleCount(maxHostSampleCount),
  _hitMaxHostSampleCount(false) {}
//_accumulated(2 * (FB_FIXED_BLOCK_SIZE + maxHostSampleCount)) {}

void 
FBOutputAggregator::Accumulate(FBFixedStereoBlock const& input)
{
  for (int zz = 0; zz < (int)_xl.size() - 1; zz++)
  {
    if (std::abs(_xl[zz] - _xl[zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }

  for (int i = 0; i < FB_FIXED_BLOCK_SIZE; i++)
    _xl.insert(_xl.end(), input[0][i]);
  for (int i = 0; i < FB_FIXED_BLOCK_SIZE; i++)
    _xr.insert(_xr.end(), input[1][i]);

  for (int zz = 0; zz < (int)_xl.size() - 1; zz++)
  {
    if (std::abs(_xl[zz] - _xl[zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }

 // input.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, FB_FIXED_BLOCK_SIZE);
 // _accumulated.sampleCount += FB_FIXED_BLOCK_SIZE;

  /*
  for (int zz = 0; zz < _accumulated.sampleCount - 1; zz++)
  {
    if (std::abs(_accumulated.audio[0][zz] - _accumulated.audio[0][zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }
  */
}

void 
FBOutputAggregator::Aggregate(FBRawStereoBlockView& output)
{
  if (_xl.size() >= _maxHostSampleCount)
    _hitMaxHostSampleCount = true;
  if (!_hitMaxHostSampleCount)
  {
    // TODO PDC
    output.Fill(0, output.Count(), 0.0f);
    return;
  }

  for (int zz = 0; zz < (int)_xl.size() - 1; zz++)
  {
    if (std::abs(_xl[zz] - _xl[zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }

  int samplesUsed = std::min(output.Count(), (int)_xl.size());
  assert(samplesUsed == output.Count());

  for (int i = 0; i < samplesUsed; i++)
  {
    output[0][i] = _xl[i];
    output[1][i] = _xr[i];
  }

  _xl.erase(_xl.begin(), _xl.begin() + samplesUsed);
  _xr.erase(_xr.begin(), _xr.begin() + samplesUsed);

  for (int zz = 0; zz < (int)_xl.size() - 1; zz++)
  {
    if (std::abs(_xl[zz] - _xl[zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      assert(false);
    }
  }
}