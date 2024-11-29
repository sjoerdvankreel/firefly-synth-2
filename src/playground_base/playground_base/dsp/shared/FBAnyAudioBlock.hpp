#pragma once

#include <cassert>

template <class DerivedT>
class FBAnyAudioBlock
{
public:
  int Count() const
  { return static_cast<DerivedT const*>(this)->Count(); }
  auto& operator[](int channel)
  { return (*static_cast<DerivedT*>(this))[channel]; }
  auto const& operator[](int channel) const
  { return (*static_cast<DerivedT const*>(this))[channel]; }

  template <class OtherDerivedT>
  void CopyFrom(
    FBAnyAudioBlock<OtherDerivedT> const& rhs, 
    int sourceOffset, int targetOffset, int count);
};

template <class DerivedT>
template <class OtherDerivedT>
void 
FBAnyAudioBlock<DerivedT>::CopyFrom(
  FBAnyAudioBlock<OtherDerivedT> const& rhs,
  int sourceOffset, int targetOffset, int count)
{
  assert(0 <= sourceOffset && sourceOffset + count <= Count());
  assert(0 <= targetOffset && targetOffset + count <= rhs.Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < count; s++)
      rhs[ch][targetOffset + s] = (*this)[ch][sourceOffset + s];
}