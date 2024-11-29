#include <playground_base/dsp/host/FBHostAudioBlock.hpp>
#include <cassert>

FBHostAudioBlock::
FBHostAudioBlock(float* l, float* r, int count) :
_count(count), _store({ l, r }) {}

void 
FBHostAudioBlock::Fill(int from, int to, float val)
{
  assert(0 <= from && from <= to && to <= _count);
  for (int ch = 0; ch < 2; ch++)
    for (int s = from; s < to; s++)
      _store[ch][s] = val;
}