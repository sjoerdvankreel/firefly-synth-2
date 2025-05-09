#include <playground_base/dsp/host/FBHostBlock.hpp>
#include <playground_base/dsp/buffer/FBBufferBlock.hpp>

#include <cassert>

void
FBBufferAudioBlock::Drop(int count)
{
  assert(0 <= count && count <= Count());
  for (int ch = 0; ch < 2; ch++)
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + count);
}

void
FBBufferAudioBlock::AppendHost(FBHostAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < rhs.Count(); s++)
      _store[ch].push_back(rhs[ch][s]);
}

void
FBBufferAudioBlock::AppendFixed(FBSIMDArray2<float, FBFixedBlockSamples, 2> const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      _store[ch].push_back(rhs[ch].Get(s));
}