#pragma once

#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <array>

template <int Count>
struct FBMonoBlock
{
  std::array<float, Count> detail_store;

  float& operator[](int index) 
  { return detail_store[index]; }
  float const& operator[](int index) const 
  { return detail_store[index]; }

  void SetToZero() 
  { detail_store.fill(0.0f); }
  void InPlaceMultiply(float x) 
  { for (int i = 0; i < Count; i++) (*this)[i] *= x; }
  void InPlaceAdd(FBMonoBlock const& rhs)
  { for (int i = 0; i < Count; i++) (*this)[i] += rhs[i]; }
};

template <int Count>
struct FBStereoBlock
{
  std::array<FBMonoBlock<Count>, FB_CHANNELS_STEREO> detail_store;

  FBMonoBlock<Count>& operator[](int index)
  { return detail_store[index]; }
  FBMonoBlock<Count> const& operator[](int index) const
  { return detail_store[index]; }

  void SetToZero() 
  { for (int i = 0; i < FB_CHANNELS_STEREO; i++) (*this)[i].SetToZero(); }
  void InPlaceMultiply(float x) 
  { for (int i = 0; i < FB_CHANNELS_STEREO; i++) (*this)[i].InPlaceMultiply(x); }
  void InPlaceAdd(FBStereoBlock const& rhs)
  { for (int i = 0; i < FB_CHANNELS_STEREO; i++) (*this)[i].InPlaceAdd(rhs[i]); }
};