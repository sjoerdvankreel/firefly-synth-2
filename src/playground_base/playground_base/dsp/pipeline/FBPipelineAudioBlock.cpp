#include <playground_base/dsp/plug/FBPlugAudioBlock.hpp>
#include <playground_base/dsp/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>
#include <cassert>

void
FBPipelineAudioBlock::Append(FBPlugAudioBlock const& plug)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < plug.Count(); s++)
      _store[ch].push_back(plug[ch][s]);
}

void
FBPipelineAudioBlock::Append(FBHostAudioBlock const& host)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < host.Count(); s++)
      _store[ch].push_back(host[ch][s]);
}

void 
FBPipelineAudioBlock::Drop(int count)
{
  assert(0 <= count && count <= Count());
  _store[0].erase(_store[0].begin(), _store[0].begin() + count);
  _store[1].erase(_store[1].begin(), _store[1].begin() + count);
}