#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBAnyAudioBlock.hpp>

#include <vector>

class FBPipelineAudioBlock:
public FBAnyAudioBlock<FBPipelineAudioBlock, std::vector<float>>
{
public:
  FBPipelineAudioBlock(): 
  FBAnyAudioBlock({}, {}) {}
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPipelineAudioBlock);

  void Drop(int count);
  int Count() const
  { return static_cast<int>(_store[0].size()); }

  template <class ThatDerivedT, class ThatStoreT>
  void Append(
    FBAnyAudioBlock<ThatDerivedT, ThatStoreT> const& that);
};

inline void
FBPipelineAudioBlock::Drop(int count)
{
  assert(0 <= count && count <= Count());
  for(int ch = 0; ch < 2; ch++)
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + count);
}

template <class ThatDerivedT, class ThatStoreT>
void FBPipelineAudioBlock::Append(
  FBAnyAudioBlock<ThatDerivedT, ThatStoreT> const& that)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < that.Count(); s++)
      _store[ch].push_back(that[ch][s]);
}