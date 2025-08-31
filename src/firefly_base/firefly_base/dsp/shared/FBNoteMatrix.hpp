#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

template <class T>
struct FBNoteFilterStateTraits;

template <>
struct FBNoteFilterStateTraits<float>
{ 
  static void Init(float& target, float val) 
  { target = val; } 
  static void DivBy(float& target, float val) 
  { target /= val; }
  static void CopyTo(float val, float& target)
  { target = val; }
};

template <>
struct FBNoteFilterStateTraits<FBSArray<float, FBFixedBlockSamples>>
{ 
  static void Init(FBSArray<float, FBFixedBlockSamples>& target, float val) 
  { target.Fill(val); } 
  static void DivBy(FBSArray<float, FBFixedBlockSamples>& target, float val) 
  { for (int s = 0; s < FBFixedBlockSamples; s++) target.Set(s, target.Get(s) / val); }
  static void CopyTo(FBSArray<float, FBFixedBlockSamples> const& val, FBSArray<float, FBFixedBlockSamples>& target)
  { val.CopyTo(target); } 
};

template <class T>
struct alignas(alignof(T)) FBNoteFilterState final
{
  T raw = {};
  T smooth = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteFilterState);

  void Init(float val)
  {
    FBNoteFilterStateTraits<T>::Init(raw, val);
    FBNoteFilterStateTraits<T>::Init(smooth, val);
  }

  void DivBy(float val)
  {
    FBNoteFilterStateTraits<T>::DivBy(raw, val);
    FBNoteFilterStateTraits<T>::DivBy(smooth, val);
  }

  void CopyTo(FBNoteFilterState& rhs) const
  {
    FBNoteFilterStateTraits<T>::CopyTo(raw, rhs.raw);
    FBNoteFilterStateTraits<T>::CopyTo(smooth, rhs.smooth);
  }
};

template <class T>
struct alignas(alignof(T)) FBNoteState final
{
  FBNoteFilterState<T> velo = {};
  FBNoteFilterState<T> keyUntuned = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteState);

  void DivKeyBy(float val)
  {
    keyUntuned.DivBy(val);
  }

  void Init(float keyUntuned_)
  {
    velo.Init(0.0f);
    keyUntuned.Init(keyUntuned_);
  }

  void CopyTo(FBNoteState& rhs) const
  {
    velo.CopyTo(rhs.velo);
    keyUntuned.CopyTo(rhs.keyUntuned);
  }
};

template <class T>
struct alignas(alignof(T)) FBNoteMatrix final
{
  FBNoteState<T> last = {};
  FBNoteState<T> lowKey = {};
  FBNoteState<T> highKey = {};
  FBNoteState<T> lowVelo = {};
  FBNoteState<T> highVelo = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteMatrix);

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

  void CopyTo(FBNoteMatrix& rhs) const
  {
    last.CopyTo(rhs.last);
    lowKey.CopyTo(rhs.lowKey);
    highKey.CopyTo(rhs.highKey);
    lowVelo.CopyTo(rhs.lowVelo);
    highVelo.CopyTo(rhs.highVelo);
  }
};

void
FBNoteMatrixInitArrayFromScalar(
  FBNoteMatrix<FBSArray<float, FBFixedBlockSamples>>& array,
  FBNoteMatrix<float> const& scalar);

void
FBNoteMatrixInitScalarFromArrayLast(
  FBNoteMatrix<float>& scalar,
  FBNoteMatrix<FBSArray<float, FBFixedBlockSamples>>& array);