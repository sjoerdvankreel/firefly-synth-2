#include <playground_base/dsp/FBOutputAggregator.hpp>

FBOutputAggregator::
FBOutputAggregator(int maxHostSampleCount) :
_hitFixedBlockSize(false),
_accumulated(maxHostSampleCount) {}

void 
FBOutputAggregator::Accumulate(FBFixedAudioBlock const& input)
{
  _accumulated.AppendFrom(input);
}

void 
FBOutputAggregator::Aggregate(FBRawAudioBlockView& output)
{
  _hitFixedBlockSize |= _accumulated.Count() >= FB_FIXED_BLOCK_SIZE;
  if (_hitFixedBlockSize)
    _accumulated.MoveOneRawBlockToAndPad(output);
  else
    output.SetToZero(0, output.Count());
}