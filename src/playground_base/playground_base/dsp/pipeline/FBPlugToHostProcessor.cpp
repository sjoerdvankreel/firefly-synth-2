#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>
#include <playground_base/dsp/pipeline/FBPlugToHostProcessor.hpp>
#include <cassert>

void 
FBPlugToHostProcessor::FromPlug(FBPlugAudioBlock const& plug)
{
  _pipeline->Append(plug);
}

void 
FBPlugToHostProcessor::ToHost(FBHostAudioBlock& host)
{
  _hitPlugBlockSize |= _pipeline->Count() >= FB_PLUG_BLOCK_SIZE;
  if (!_hitPlugBlockSize)
  {
    host.Fill(0, host.Count(), 0.0f);
    return;
  }

  int padded = std::max(0, host.Count() - _pipeline->Count());
  assert(!_paddedOnce || padded == 0);
  _paddedOnce |= padded > 0;
  host.Fill(0, padded, 0.0f);

  int used = std::min(host.Count(), _pipeline->Count());
  host.CopyFrom(*_pipeline, padded, used);
  _pipeline->Drop(used);
}