#include <playground_base/dsp/FBOutputAggregator.hpp>

FBOutputAggregator::
FBOutputAggregator(int maxHostSampleCount):
_accumulated(FB_FIXED_BLOCK_SIZE + maxHostSampleCount) {}

void 
FBOutputAggregator::Accumulate(FBFixedStereoBlock const& input)
{
  if(_accumulated.sampleCount!=0)
    if (std::abs(input[0][0] - _accumulated.audio[0][_accumulated.sampleCount - 1]) > 0.1)
    {
      int x = 0;
      x++;
      assert(false);
    }

  for (int i = 0; i < input.Count() - 1; i++)
  {
    if (std::abs(input[0][i] - input[0][i + 1]) > 0.1)
    {
      int x = 0;
      x++;
      assert(false);
    }
  }

  for (int i = 0; i < _accumulated.sampleCount - 1; i++)
  {
    if (std::abs(_accumulated.audio[0][i] - _accumulated.audio[0][i + 1]) > 0.1)
    {
      int x = 0;
      x++;
      assert(false);
    }
  }

  input.CopyTo(_accumulated.audio, 0, _accumulated.sampleCount, FB_FIXED_BLOCK_SIZE);
  _accumulated.sampleCount += FB_FIXED_BLOCK_SIZE;

  for (int i = 0; i < _accumulated.sampleCount - 1; i++)
  {
    if (std::abs(_accumulated.audio[0][i] - _accumulated.audio[0][i + 1]) > 0.1)
    {
      int x = 0;
      x++;
      assert(false);
    }
  }
}

void 
FBOutputAggregator::Aggregate(FBRawStereoBlockView& output)
{
  int samplesUsed = std::min(output.Count(), _accumulated.sampleCount);
  int samplesPadded = std::max(0, output.Count() - _accumulated.sampleCount);

  for (int i = 0; i < _accumulated.sampleCount - 1; i++)
  {
    if (std::abs(_accumulated.audio[0][i] - _accumulated.audio[0][i + 1]) > 0.1)
    {
      int x = 0;
      x++;
      assert(false);
    }
  }
  
  output.Fill(0, samplesPadded, 0.0f);
  _accumulated.audio.CopyTo(output, 0, samplesPadded, samplesUsed);
  _accumulated.audio.ShiftLeft(samplesUsed);
  _accumulated.sampleCount -= samplesUsed;

  for (int i = 0; i < _accumulated.sampleCount - 1; i++)
  {
    if (std::abs(_accumulated.audio[0][i] - _accumulated.audio[0][i + 1]) > 0.1)
    {
      int x = 0;
      x++;
      assert(false);
    }
  }

  for (int i = 0; i < output.Count() - 1; i++)
  {
    if (std::abs(output[0][i] - output[0][i + 1]) > 0.1)
    {
      int x = 0;
      x++;
    }
  }
}