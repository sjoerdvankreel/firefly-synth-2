#include <playground_base/pipeline/FBPlugToHostProcessor.hpp>

void 
FBPlugToHostProcessor::FromPlug(FBPlugAudioBlock const& plug)
{
  _pipeline.AppendFrom(plug);
}

void 
FBPlugToHostProcessor::ToHost(FBHostAudioBlock& host)
{
  _hitPlugBlockSize |= _pipeline.Count() >= FB_PLUG_BLOCK_SIZE;
  if (_hitPlugBlockSize)
    _pipeline.MoveOneRawBlockToAndPad(host);
  else
    host.SetToZero(0, host.Count());
}