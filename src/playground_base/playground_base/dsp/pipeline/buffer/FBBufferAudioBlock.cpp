#include <playground_base/dsp/pipeline/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

#include <cassert>

void
FBBufferAudioBlock::Drop(int count)
{
  assert(0 <= count && count <= Count());
  for (int ch = 0; ch < 2; ch++)
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + count);
}

void
FBBufferAudioBlock::Append(FBHostAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < rhs.Count(); s++)
      _store[ch].push_back(rhs[ch][s]);
}

void
FBBufferAudioBlock::Append(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
  {
    _store[ch].insert(_store[ch].end(), FBFixedBlockSamples, 0.0f);
    rhs[ch].StoreUnaligned(_store[ch].data() + _store[ch].size() - FBFixedBlockSamples);
  }
}