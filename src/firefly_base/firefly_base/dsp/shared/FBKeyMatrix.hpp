#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

template <class T>
struct FBKeyFilterStateTraits;

template <>
struct FBKeyFilterStateTraits<float>
{ 
  static void 
  Init(float& target, float val) 
  { 
    target = val;
  } 
};

template <>
struct FBKeyFilterStateTraits<FBSArray<float, FBFixedBlockSamples>>
{ 
  static void 
  Init(FBSArray<float, FBFixedBlockSamples>& target, float val) 
  { 
    target.Fill(val);
  } 
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
};

template <class T>
struct alignas(alignof(T)) FBKeyState final
{
  FBKeyFilterState<T> velo = {};
  FBKeyFilterState<T> keyUntuned = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBKeyState);

  void Init(float keyUntuned_)
  {
    velo.Init(0.0f);
    keyUntuned.Init(keyUntuned_);
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

  void Init(float keyUntuned)
  {
    last.Init(keyUntuned);
    lowKey.Init(keyUntuned);
    highKey.Init(keyUntuned);
    lowVelo.Init(keyUntuned);
    highVelo.Init(keyUntuned);
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