#include <playground_base/dsp/FBOutputAggregator.hpp>

FBOutputAggregator::
FBOutputAggregator(int maxHostSampleCount) :
_hitFixedBlockSize(false),
_maxHostSampleCount(maxHostSampleCount) {}
//_accumulated(2 * (FB_FIXED_BLOCK_SIZE + maxHostSampleCount)) {}

void 
FBOutputAggregator::Accumulate(FBFixedAudioBlock const& input)
{
  for (int zz = 0; zz < (int)_xl.size() - 1; zz++)
  {
    if (std::abs(_xl[zz] - _xl[zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      //assert(false);
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
      //assert(false);
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
      //assert(false);
    }
  }
  */
}

void 
FBOutputAggregator::Aggregate(FBRawAudioBlockView& output)
{
  if (_xl.size() >= FB_FIXED_BLOCK_SIZE)
    _hitFixedBlockSize = true;
  if (!_hitFixedBlockSize)
  {
    // TODO PDC
    output.SetToZero(0, output.Count());
    return;
  }

  for (int zz = 0; zz < (int)_xl.size() - 1; zz++)
  {
    if (std::abs(_xl[zz] - _xl[zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      //assert(false);
    }
  }

  int samplesUsed = std::min(output.Count(), (int)_xl.size());
  int samplesPadded = std::max(0, output.Count() - samplesUsed);
  assert(samplesPadded + samplesUsed == output.Count());

  output.SetToZero(0, samplesPadded);
  /*
  for (int i = 0; i < samplesPadded; i++)
  {
    output[0][i] = 0;
    output[1][i] = 0;
  }
  */

  for (int i = samplesPadded; i < samplesPadded + samplesUsed; i++)
  {
    output[0][i] = _xl[i - samplesPadded];
    output[1][i] = _xr[i - samplesPadded];
  }

  _xl.erase(_xl.begin(), _xl.begin() + samplesUsed);
  _xr.erase(_xr.begin(), _xr.begin() + samplesUsed);

  for (int zz = 0; zz < (int)_xl.size() - 1; zz++)
  {
    if (std::abs(_xl[zz] - _xl[zz + 1]) > 0.1)
    {
      int zzz = 0;
      zzz++;
      //assert(false);
    }
  }
}