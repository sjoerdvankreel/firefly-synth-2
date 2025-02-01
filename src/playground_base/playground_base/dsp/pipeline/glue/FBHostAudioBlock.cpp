#include <playground_base/dsp/pipeline/glue/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

#include <cassert>

FBHostAudioBlock::
FBHostAudioBlock(float** channels, int count):
_count(count), _store({ channels[0], channels[1] }) {}

void 
FBHostAudioBlock::CopyFrom(
  FBBufferAudioBlock const& rhs, int count)
{
  assert(count <= Count());
  assert(count <= rhs.Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < count; s++)
      _store[ch][0 + s] = rhs[ch][s];
}