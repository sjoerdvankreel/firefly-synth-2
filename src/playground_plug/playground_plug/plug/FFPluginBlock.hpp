
#if 0
#pragma once

#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>

struct FBProcessorContext
{
  int moduleSlot;
  float sampleRate;
};

template <int Count>
struct alignas(Count*sizeof(float)) FBMonoBlock
{
  std::array<float, Count> store;
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FBMonoBlock);

  float& operator[](int index) 
  { return store[index]; }
  float const& operator[](int index) const 
  { return store[index]; }

  void SetToZero() 
  { store.fill(0.0f); }
  void CopyTo(FBMonoBlock& rhs) const
  { for (int i = 0; i < Count; i++) rhs[i] = (*this)[i]; }

  void InPlaceMultiply(float x) 
  { for (int i = 0; i < Count; i++) (*this)[i] *= x; }
  void InPlaceAdd(FBMonoBlock const& rhs)
  { for (int i = 0; i < Count; i++) (*this)[i] += rhs[i]; }
};

template <int Count>
struct alignas(Count * sizeof(float)) FBStereoBlock
{
  std::array<FBMonoBlock<Count>, FB_CHANNELS_STEREO> store;
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FBStereoBlock);

  FBMonoBlock<Count>& operator[](int index)
  { return store[index]; }
  FBMonoBlock<Count> const& operator[](int index) const
  { return store[index]; }

  void SetToZero() 
  { for (int i = 0; i < FB_CHANNELS_STEREO; i++) (*this)[i].SetToZero(); }
  void CopyTo(FBStereoBlock& rhs) const
  { for (int i = 0; i < FB_CHANNELS_STEREO; i++) (*this)[i].CopyTo(rhs[i]); }

  void InPlaceMultiply(float x) 
  { for (int i = 0; i < FB_CHANNELS_STEREO; i++) (*this)[i].InPlaceMultiply(x); }
  void InPlaceAdd(FBStereoBlock const& rhs)
  { for (int i = 0; i < FB_CHANNELS_STEREO; i++) (*this)[i].InPlaceAdd(rhs[i]); }
};

#endif