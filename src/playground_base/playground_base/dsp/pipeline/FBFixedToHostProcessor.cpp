#include <playground_base/dsp/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/fixed/FBFixedAudioBlock.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>
#include <playground_base/dsp/pipeline/FBFixedToHostProcessor.hpp>

#include <cassert>

void 
FBFixedToHostProcessor::FromFixed(FBFixedAudioBlock const& fixed)
{
  _pipeline->Append(fixed);
}

void 
FBFixedToHostProcessor::ToHost(FBHostAudioBlock& host)
{
  _hitFixedBlockSize |= _pipeline->Count() >= FBFixedAudioBlock::Count();
  if (!_hitFixedBlockSize)
  {
    host.Fill(0, host.Count(), 0.0f);
    return;
  }

  int padded = std::max(0, host.Count() - _pipeline->Count());
  assert(!_paddedOnce || padded == 0);
  _paddedOnce |= padded > 0;
  host.Fill(0, padded, 0.0f);

  int used = std::min(host.Count(), _pipeline->Count());
  host.CopyFrom(*_pipeline, padded, 0, used);
  _pipeline->Drop(used);
}