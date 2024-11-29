#pragma once

#include <playground_base/base/shared/FBMixin.hpp>
#include <cassert>

template <class DerivedT>
class FBAnyAudioBlock:
public FBMixin<FBAnyAudioBlock<DerivedT>>
{
public:
  int Count() const
  { return This().Count(); }
  auto& operator[](int channel)
  { return This()[channel]; }
  auto const& operator[](int channel) const
  { return This()[channel]; }

  template <class ThatDerivedT>
  void CopyFrom(
    FBAnyAudioBlock<ThatDerivedT> const& that,
    int thisOffset, int thatOffset, int count);
};

template <class DerivedT>
template <class ThatDerivedT>
void 
FBAnyAudioBlock<DerivedT>::CopyFrom(
  FBAnyAudioBlock<ThatDerivedT> const& that,
  int thisOffset, int thatOffset, int count)
{
  assert(0 <= thisOffset && thisOffset + count <= Count());
  assert(0 <= thatOffset && thatOffset + count <= that.Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < count; s++)
      (*this)[ch][thisOffset + s] = that[ch][thatOffset + s];
}