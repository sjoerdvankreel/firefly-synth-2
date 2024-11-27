#include <playground_base/pipeline/FBOutputAccumulator.hpp>

FBOutputAccumulator::
FBOutputAccumulator(int maxHostSampleCount) :
_hitFixedBlockSize(false),
_accumulated(maxHostSampleCount) {}

void 
FBOutputAccumulator::AccumulateFrom(FBFixedAudioBlock const& input)
{
  _accumulated.AppendFrom(input);
}

void 
FBOutputAccumulator::AggregateTo(FBRawAudioBlockView& output)
{
  _hitFixedBlockSize |= _accumulated.Count() >= FB_FIXED_BLOCK_SIZE;
  if (_hitFixedBlockSize)
    _accumulated.MoveOneRawBlockToAndPad(output);
  else
    output.SetToZero(0, output.Count());
}