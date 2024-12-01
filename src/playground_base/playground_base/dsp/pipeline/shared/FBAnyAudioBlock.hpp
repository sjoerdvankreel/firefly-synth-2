#pragma once

#include <playground_base/base/shared/FBMixin.hpp>

#include <array>
#include <utility>
#include <cassert>

template <class DerivedT, class StoreT>
class FBAnyAudioBlock:
public FBMixin<DerivedT>
{
protected:
  typedef StoreT StoreT;
  std::array<StoreT, 2> _store;

  StoreT& operator[](int channel)
  { return _store[channel]; }
  FBAnyAudioBlock(StoreT&& l, StoreT&& r) :
  _store({ std::move(l), std::move(r) }) {}

public:
  int Count() const
  { return this->Self().Count(); }
  StoreT const& operator[](int channel) const
  { return _store[channel]; }

  void Fill(int from, int to, float val);
  template <class ThatDerivedT, class ThatStoreT>
  void CopyFrom(
    FBAnyAudioBlock<ThatDerivedT, ThatStoreT> const& that,
    int thisOffset, int thatOffset, int count);
};

template <class DerivedT, class StoreT>
void FBAnyAudioBlock<DerivedT, StoreT>::Fill(
  int from, int to, float val)
{
  assert(0 <= from && from <= to && to <= Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = from; s < to; s++)
      (*this)[ch][s] = val;
}

template <class DerivedT, class StoreT>
template <class ThatDerivedT, class ThatStoreT>
void 
FBAnyAudioBlock<DerivedT, StoreT>::CopyFrom(
  FBAnyAudioBlock<ThatDerivedT, ThatStoreT> const& that,
  int thisOffset, int thatOffset, int count)
{
  assert(0 <= thisOffset && thisOffset + count <= Count());
  assert(0 <= thatOffset && thatOffset + count <= that.Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < count; s++)
      (*this)[ch][thisOffset + s] = that[ch][thatOffset + s];
}