#include <playground_base/dsp/pipeline/host/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

#include <cassert>

FBHostAudioBlock::
FBHostAudioBlock(float** channels, int count):
_count(count), _store({ channels[0], channels[1] }) {}

void
FBHostAudioBlock::SetToZero(int from, int to)
{
  assert(0 <= from && from <= to && to <= Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = from; s < to; s++)
      _store[ch][s] = 0.0f;
}

void 
FBHostAudioBlock::CopyFrom(
  FBBufferAudioBlock const& rhs, int thisStart, int thatCount)
{
  assert(thatCount <= rhs.Count());
  assert(0 <= thisStart && thisStart + thatCount <= Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < thatCount; s++)
      _store[ch][thisStart + s] = rhs[ch][s];
}