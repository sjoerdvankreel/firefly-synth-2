#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBAnyAudioBlock.hpp>

#include <vector>

class FBBufferAudioBlock final:
public FBAnyAudioBlock<FBBufferAudioBlock, std::vector<float>>
{
public:
  FBBufferAudioBlock():
  FBAnyAudioBlock({}, {}) {}
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBBufferAudioBlock);

  void Drop(int count);
  int Count() const
  { return static_cast<int>(_store[0].size()); }

  template <class ThatDerivedT, class ThatStoreT>
  void Append(
    FBAnyAudioBlock<ThatDerivedT, ThatStoreT> const& that);
};

inline void
FBBufferAudioBlock::Drop(int count)
{
  assert(0 <= count && count <= Count());
  for(int ch = 0; ch < 2; ch++)
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + count);
}

template <class ThatDerivedT, class ThatStoreT>
void FBBufferAudioBlock::Append(
  FBAnyAudioBlock<ThatDerivedT, ThatStoreT> const& that)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < that.Count(); s++)
      _store[ch].push_back(that[ch][s]);
}