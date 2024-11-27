#include <playground_base/pipeline/FBOutputAccumulator.hpp>

FBOutputAccumulator::
FBOutputAccumulator(int maxHostSampleCount) :
_hitFixedBlockSize(false),
_pipeline(maxHostSampleCount) {}

void 
FBOutputAccumulator::AccumulateFrom(FBPlugAudioBlock const& input)
{
  _pipeline.AppendFrom(input);
}

void 
FBOutputAccumulator::AggregateTo(FBHostAudioBlock& output)
{
  _hitFixedBlockSize |= _pipeline.Count() >= FB_PLUG_BLOCK_SIZE;
  if (_hitFixedBlockSize)
    _pipeline.MoveOneRawBlockToAndPad(output);
  else
    output.SetToZero(0, output.Count());
}