#include <firefly_base/dsp/host/FBHostBlock.hpp>
#include <firefly_base/dsp/buffer/FBBufferBlock.hpp>

FBHostAudioBlock::
FBHostAudioBlock(float** channels, int count) :
  _count(count), _store({ { channels[0], channels[1] } }) {}

void
FBHostAudioBlock::CopyFrom(
  FBBufferAudioBlock const& rhs, int count)
{
  FB_ASSERT(count <= Count());
  FB_ASSERT(count <= rhs.Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < count; s++)
      _store[ch][0 + s] = rhs[ch][s];
}