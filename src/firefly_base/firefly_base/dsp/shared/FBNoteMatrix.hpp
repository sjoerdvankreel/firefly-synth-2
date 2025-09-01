#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

enum class FBNoteMatrixEntry
{
  LastVeloRaw,
  LastVeloSmooth,
  LowKeyVeloRaw,
  LowKeyVeloSmooth,
  HighKeyVeloRaw,
  HighKeyVeloSmooth,
  LowVeloVeloRaw,
  LowVeloVeloSmooth,
  HighVeloVeloRaw,
  HighVeloVeloSmooth,
  VeloCount,

  LastKeyUntunedRaw = VeloCount,
  LastKeyUntunedSmooth,
  LowKeyKeyUntunedRaw,
  LowKeyKeyUntunedSmooth,
  HighKeyKeyUntunedRaw,
  HighKeyKeyUntunedSmooth,
  LowVeloKeyUntunedRaw,
  LowVeloKeyUntunedSmooth,
  HighVeloKeyUntunedRaw,
  HighVeloKeyUntunedSmooth,
  Count
};

template <class T>
struct FBNoteMatrixTraits;

template <>
struct FBNoteMatrixTraits<float>
{
  static void Set(float& target, float val) { target = val; }
  static void Div(float& target, float val) { target /= val; }
  static void CopyTo(float source, float& target) { target = source; }
};

template <>
struct FBNoteMatrixTraits<FBSArray<float, FBFixedBlockSamples>>
{
  static void Set(FBSArray<float, FBFixedBlockSamples>& target, float val) {
    for (int i = 0; i < FBFixedBlockSamples; i++) target.Set(i, val); }
  static void Div(FBSArray<float, FBFixedBlockSamples>& target, float val) {
    for (int i = 0; i < FBFixedBlockSamples; i++) target.Set(i, target.Get(i) / val); }
  static void CopyTo(
    FBSArray<float, FBFixedBlockSamples> const& source, 
    FBSArray<float, FBFixedBlockSamples>& target) {
    source.CopyTo(target); }
};

template <class T>
struct alignas(alignof(T)) FBNoteMatrix final
{
  std::array<T, (int)FBNoteMatrixEntry::Count> entries = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteMatrix);

  void DivKeyBy(float val)
  {
    for (int i = (int)FBNoteMatrixEntry::VeloCount; i < (int)FBNoteMatrixEntry::Count; i++)
      FBNoteMatrixTraits<T>::Div(entries[i], val);
  }

  void SetKey(float keyUntuned)
  {
    for (int i = (int)FBNoteMatrixEntry::VeloCount; i < (int)FBNoteMatrixEntry::Count; i++)
      FBNoteMatrixTraits<T>::Set(entries[i], keyUntuned);
  }

  void CopyTo(FBNoteMatrix& rhs) const
  {
    for (int i = 0; i < (int)FBNoteMatrixEntry::Count; i++)
      FBNoteMatrixTraits<T>::CopyTo(entries[i], rhs.entries[i]);
  }
};

inline void
FBNoteMatrixInitArrayFromScalar(
  FBNoteMatrix<FBSArray<float, FBFixedBlockSamples>>& array,
  FBNoteMatrix<float> const& scalar)
{
  for (int i = 0; i < (int)FBNoteMatrixEntry::Count; i++)
    array.entries[i].Fill(scalar.entries[i]);
}

inline void
FBNoteMatrixInitScalarFromArrayLast(
  FBNoteMatrix<float>& scalar,
  FBNoteMatrix<FBSArray<float, FBFixedBlockSamples>>& array)
{
  for (int i = 0; i < (int)FBNoteMatrixEntry::Count; i++)
    scalar.entries[i] = array.entries[i].Last();
}