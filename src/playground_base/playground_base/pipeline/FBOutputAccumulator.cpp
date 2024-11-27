#include <playground_base/pipeline/FBOutputAccumulator.hpp>

FBOutputAccumulator::
FBOutputAccumulator(int maxHostSampleCount) :
_hitFixedBlockSize(false),
_accumulated(maxHostSampleCount) {}

void 
FBOutputAccumulator::AccumulateFrom(FBPlugAudioBlock const& input)
{
  _accumulated.AppendFrom(input);
}

void 
FBOutputAccumulator::AggregateTo(FBHostAudioBlock& output)
{
  _hitFixedBlockSize |= _accumulated.Count() >= FB_PLUG_BLOCK_SIZE;
  if (_hitFixedBlockSize)
    _accumulated.MoveOneRawBlockToAndPad(output);
  else
    output.SetToZero(0, output.Count());
}