#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/buffer/FBBufferBlock.hpp>

void
FBBufferAudioBlock::Drop(int count)
{
  FB_ASSERT(0 <= count && count <= Count());
  for (int ch = 0; ch < 2; ch++)
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + count);
}

void
FBBufferAudioBlock::AppendHostAudio(
  FBHostAudioBlock const& hostAudio)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < hostAudio.Count(); s++)
      _store[ch].push_back(hostAudio[ch][s]);
}

void
FBBufferAudioBlock::AppendPlugAudio(
  FBSArray2<float, FBFixedBlockSamples, 2> const& plugAudio)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      _store[ch].push_back(plugAudio[ch].Get(s));
}