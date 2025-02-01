#include <playground_base/dsp/pipeline/glue/FBHostAudioBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
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
FBBufferAudioBlock::AppendHost(FBHostAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < rhs.Count(); s++)
      _store[ch].push_back(rhs[ch][s]);
}

void
FBBufferAudioBlock::AppendFixed(FBFixedFloatAudioBlock const& rhs)
{
  FBFixedFloatAudioArray array;
  rhs.StoreToFloatArray(array);
  for (int ch = 0; ch < 2; ch++)
    _store[ch].insert(_store[ch].end(), array.data[ch].data.begin(), array.data[ch].data.end());
}