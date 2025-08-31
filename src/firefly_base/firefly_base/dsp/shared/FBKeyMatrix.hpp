#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

template <class T>
struct FBKeyFilterStateTraits;

template <>
struct FBKeyFilterStateTraits<float>
{ 
  static void Init(float& target, float val) 
  { target = val; } 
  static void DivBy(float& target, float val) 
  { target /= val; }
  static void CopyTo(float val, float& target)
  { target = val; }
};

template <>
struct FBKeyFilterStateTraits<FBSArray<float, FBFixedBlockSamples>>
{ 
  static void Init(FBSArray<float, FBFixedBlockSamples>& target, float val) 
  { target.Fill(val); } 
  static void DivBy(FBSArray<float, FBFixedBlockSamples>& target, float val) 
  { for (int s = 0; s < FBFixedBlockSamples; s++) target.Set(s, target.Get(s) / val); }
  static void CopyTo(FBSArray<float, FBFixedBlockSamples> const& val, FBSArray<float, FBFixedBlockSamples>& target)
  { val.CopyTo(target); } 
};

template <class T>
struct alignas(alignof(T)) FBKeyFilterState final
{
  T raw = {};
  T smooth = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBKeyFilterState);

  void Init(float val)
  {
    FBKeyFilterStateTraits<T>::Init(raw, val);
    FBKeyFilterStateTraits<T>::Init(smooth, val);
  }

  void DivBy(float val)
  {
    FBKeyFilterStateTraits<T>::DivBy(raw, val);
    FBKeyFilterStateTraits<T>::DivBy(smooth, val);
  }

  void CopyTo(FBKeyFilterState& rhs) const
  {
    FBKeyFilterStateTraits<T>::CopyTo(raw, rhs.raw);
    FBKeyFilterStateTraits<T>::CopyTo(smooth, rhs.smooth);
  }
};

template <class T>
struct alignas(alignof(T)) FBKeyState final
{
  FBKeyFilterState<T> velo = {};
  FBKeyFilterState<T> keyUntuned = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBKeyState);

  void DivKeyBy(float val)
  {
    keyUntuned.DivBy(val);
  }

  void Init(float keyUntuned_)
  {
    velo.Init(0.0f);
    keyUntuned.Init(keyUntuned_);
  }

  void CopyTo(FBKeyState& rhs) const
  {
    velo.CopyTo(rhs.velo);
    keyUntuned.CopyTo(rhs.keyUntuned);
  }
};

template <class T>
struct alignas(alignof(T)) FBKeyMatrix final
{
  FBKeyState<T> last = {};
  FBKeyState<T> lowKey = {};
  FBKeyState<T> highKey = {};
  FBKeyState<T> lowVelo = {};
  FBKeyState<T> highVelo = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBKeyMatrix);

  void DivKeyBy(float val)
  {
    last.DivKeyBy(val);
    lowKey.DivKeyBy(val);
    highKey.DivKeyBy(val);
    lowVelo.DivKeyBy(val);
    highVelo.DivKeyBy(val);
  }

  void Init(float keyUntuned)
  {
    last.Init(keyUntuned);
    lowKey.Init(keyUntuned);
    highKey.Init(keyUntuned);
    lowVelo.Init(keyUntuned);
    highVelo.Init(keyUntuned);
  }

  void CopyTo(FBKeyMatrix& rhs) const
  {
    last.CopyTo(rhs.last);
    lowKey.CopyTo(rhs.lowKey);
    highKey.CopyTo(rhs.highKey);
    lowVelo.CopyTo(rhs.lowVelo);
    highVelo.CopyTo(rhs.highVelo);
  }
};

void
FBKeyMatrixInitArrayFromScalar(
  FBKeyMatrix<FBSArray<float, FBFixedBlockSamples>>& array,
  FBKeyMatrix<float> const& scalar);

void
FBKeyMatrixInitScalarFromArrayLast(
  FBKeyMatrix<float>& scalar,
  FBKeyMatrix<FBSArray<float, FBFixedBlockSamples>>& array);