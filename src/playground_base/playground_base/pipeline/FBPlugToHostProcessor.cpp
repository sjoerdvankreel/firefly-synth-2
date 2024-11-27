#include <playground_base/pipeline/FBPlugToHostProcessor.hpp>
#include <playground_base/shared/FBHostBlock.hpp>

FBPlugToHostProcessor::
FBPlugToHostProcessor():
_hitPlugBlockSize(false),
_pipeline(std::make_unique<FBPipelineAudioBlock>()) {}

void 
FBPlugToHostProcessor::FromPlug(FBPlugAudioBlock const& plug)
{
  for (int s = 0; s < FB_PLUG_BLOCK_SIZE; s++)
    _pipeline->Append(plug[0][s], plug[1][s]);
}

void 
FBPlugToHostProcessor::ToHost(FBHostAudioBlock& host)
{
  _hitPlugBlockSize |= _pipeline->Count() >= FB_PLUG_BLOCK_SIZE;
  if (_hitPlugBlockSize)
    _pipeline->MoveOneRawBlockToAndPad(host);
  else
    host.SetToZero(0, host.Count());
}