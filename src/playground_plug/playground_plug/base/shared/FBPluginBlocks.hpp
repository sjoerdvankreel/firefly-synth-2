#pragma once

#include <array>
#include <cstddef>

template <int Count>
struct FBSingleBlock
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
  void InPlaceAdd(FBSingleBlock const& rhs) 
  { for (int i = 0; i < Count; i++) (*this)[i] += rhs[i]; }
  void CopyTo(float* destination, int count) 
  { for (int i = 0; i < count; i++) destination[i] = (*this)[i]; }
};

template <int Count>
struct FBDualBlock
{
  std::array<FBSingleBlock<Count>, 2> detail_store;

  FBSingleBlock<Count>& operator[](int index) 
  { return detail_store[index]; }
  FBSingleBlock<Count> const& operator[](int index) const 
  { return detail_store[index]; }

  void SetToZero() 
  { for (int i = 0; i < 2; i++) (*this)[i].SetToZero(); }
  void InPlaceMultiply(float x) 
  { for (int i = 0; i < 2; i++) (*this)[i].InPlaceMultiply(x); }
  void InPlaceAdd(FBDualBlock const& rhs) 
  { for (int i = 0; i < 2; i++) (*this)[i].InPlaceAdd(rhs[i]); }
  void CopyTo(float* const* destination, int count) 
  { for (int i = 0; i < 2; i++) (*this)[i].CopyTo(destination[i]); }
};