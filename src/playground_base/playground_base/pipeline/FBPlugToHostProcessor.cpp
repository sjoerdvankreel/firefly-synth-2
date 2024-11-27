#include <playground_base/pipeline/FBPlugToHostProcessor.hpp>

void 
FBPlugToHostProcessor::AccumulateFrom(FBPlugAudioBlock const& input)
{
  _pipeline.AppendFrom(input);
}

void 
FBPlugToHostProcessor::AggregateTo(FBHostAudioBlock& output)
{
  _hitFixedBlockSize |= _pipeline.Count() >= FB_PLUG_BLOCK_SIZE;
  if (_hitFixedBlockSize)
    _pipeline.MoveOneRawBlockToAndPad(output);
  else
    output.SetToZero(0, output.Count());
}