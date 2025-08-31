#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

template <class T>
struct FBKeyFilterStateTraits;

template <>
struct FBKeyFilterStateTraits<float>
{ 
  static void 
  SetKeyUntuned(float& keyUntuned, float val) 
  { 
    keyUntuned = val;
  } 
};

template <>
struct FBKeyFilterStateTraits<FBSArray<float, FBFixedBlockSamples>>
{ 
  static void 
  SetKeyUntuned(FBSArray<float, FBFixedBlockSamples>& keyUntuned, float val) 
  { 
    keyUntuned.Fill(val); 
  } 
};

template <class T>
struct alignas(alignof(T)) FBKeyFilterState final
{
  T raw = {};
  T smooth = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBKeyFilterState);

  void SetKeyUntuned(float val) 
  { 
    FBKeyFilterStateTraits<T>::SetKeyUntuned(raw, val);
    FBKeyFilterStateTraits<T>::SetKeyUntuned(smooth, val);
  }
};

template <class T>
struct alignas(alignof(T)) FBKeyState final
{
  FBKeyFilterState<T> velo = {};
  FBKeyFilterState<T> keyUntuned = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBKeyState);

  void SetKeyUntuned(float val)
  {
    keyUntuned.SetKeyUntuned(val);
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

  void SetKeyUntuned(float val)
  {
    last.SetKeyUntuned(val);
    lowKey.SetKeyUntuned(val);
    highKey.SetKeyUntuned(val);
    lowVelo.SetKeyUntuned(val);
    highVelo.SetKeyUntuned(val);
  }
};

inline void 
FBKeyMatrixInitFromScalar(
  FBKeyMatrix<FBSArray<float, FBFixedBlockSamples>>& matrix,
  FBKeyMatrix<float> const& scalar)
{
  matrix.last.velo.raw.Fill(scalar.last.velo.raw);
  matrix.last.velo.smooth.Fill(scalar.last.velo.smooth);
  matrix.last.keyUntuned.raw.Fill(scalar.last.keyUntuned.raw);
  matrix.last.keyUntuned.smooth.Fill(scalar.last.keyUntuned.smooth);
  matrix.lowKey.velo.raw.Fill(scalar.lowKey.velo.raw);
  matrix.lowKey.velo.smooth.Fill(scalar.lowKey.velo.smooth);
  matrix.lowKey.keyUntuned.raw.Fill(scalar.lowKey.keyUntuned.raw);
  matrix.lowKey.keyUntuned.smooth.Fill(scalar.lowKey.keyUntuned.smooth);
  matrix.highKey.velo.raw.Fill(scalar.highKey.velo.raw);
  matrix.highKey.velo.smooth.Fill(scalar.highKey.velo.smooth);
  matrix.highKey.keyUntuned.raw.Fill(scalar.highKey.keyUntuned.raw);
  matrix.highKey.keyUntuned.smooth.Fill(scalar.highKey.keyUntuned.smooth);
  matrix.lowVelo.velo.raw.Fill(scalar.lowVelo.velo.raw);
  matrix.lowVelo.velo.smooth.Fill(scalar.lowVelo.velo.smooth);
  matrix.lowVelo.keyUntuned.raw.Fill(scalar.lowVelo.keyUntuned.raw);
  matrix.lowVelo.keyUntuned.smooth.Fill(scalar.lowVelo.keyUntuned.smooth);
  matrix.highVelo.velo.raw.Fill(scalar.highVelo.velo.raw);
  matrix.highVelo.velo.smooth.Fill(scalar.highVelo.velo.smooth);
  matrix.highVelo.keyUntuned.raw.Fill(scalar.highVelo.keyUntuned.raw);
  matrix.highVelo.keyUntuned.smooth.Fill(scalar.highVelo.keyUntuned.smooth);
}