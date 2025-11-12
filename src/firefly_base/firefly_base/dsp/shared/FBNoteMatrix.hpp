#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

enum class FBNoteVeloMatrixEntry
{
  LastVelo,
  LowKeyVelo,
  HighKeyVelo,
  LowVeloVelo,
  HighVeloVelo,
  Count
};

enum class FBNoteKeyMatrixEntry
{
  LastKey,
  LowKeyKey,
  HighKeyKey,
  LowVeloKey,
  HighVeloKey,
  Count
};

template <class T>
struct FBNoteMatrixTraits;

template <>
struct FBNoteMatrixTraits<float>
{
  static void Set(float& target, float val) { target = val; }
  static void CopyTo(float source, float& target) { target = source; }
};

template <>
struct FBNoteMatrixTraits<FBSArray<float, FBFixedBlockSamples>>
{
  static void Set(FBSArray<float, FBFixedBlockSamples>& target, float val) {
    for (int i = 0; i < FBFixedBlockSamples; i++) target.Set(i, val); }
  static void CopyTo(
    FBSArray<float, FBFixedBlockSamples> const& source, 
    FBSArray<float, FBFixedBlockSamples>& target) {
    source.CopyTo(target); }
};

template <class T>
struct alignas(alignof(T)) FBNoteVeloMatrix final
{
  std::array<T, (int)FBNoteVeloMatrixEntry::Count> entries = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteVeloMatrix);

  void CopyTo(FBNoteVeloMatrix& rhs) const {
    for (int i = 0; i < (int)FBNoteVeloMatrixEntry::Count; i++)
      FBNoteMatrixTraits<T>::CopyTo(entries[i], rhs.entries[i]); }
};

template <class T>
struct alignas(alignof(T)) FBNoteKeyMatrix final
{
  std::array<T, (int)FBNoteKeyMatrixEntry::Count> entries = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteKeyMatrix);

  void Set(float key) {
    for (int i = 0; i < (int)FBNoteKeyMatrixEntry::Count; i++)
      FBNoteMatrixTraits<T>::Set(entries[i], key); }

  void CopyTo(FBNoteKeyMatrix& rhs) const {
    for (int i = 0; i < (int)FBNoteKeyMatrixEntry::Count; i++)
      FBNoteMatrixTraits<T>::CopyTo(entries[i], rhs.entries[i]); }
};

template <class T>
struct alignas(alignof(T)) FBNoteMatrix final
{
  FBNoteKeyMatrix<T> key = {};
  FBNoteVeloMatrix<T> velo = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteMatrix);

  void CopyTo(FBNoteMatrix& rhs) const {
    key.CopyTo(rhs.key);
    velo.CopyTo(rhs.velo);
  }
};

inline void
FBNoteVeloMatrixInitArrayFromScalar(
  FBNoteVeloMatrix<FBSArray<float, FBFixedBlockSamples>>& array,
  FBNoteVeloMatrix<float> const& scalar)
{
  for (int i = 0; i < (int)FBNoteVeloMatrixEntry::Count; i++)
    array.entries[i].Fill(scalar.entries[i]);
}

inline void
FBNoteVeloMatrixInitScalarFromArrayLast(
  FBNoteVeloMatrix<float>& scalar,
  FBNoteVeloMatrix<FBSArray<float, FBFixedBlockSamples>> const& array)
{
  for (int i = 0; i < (int)FBNoteVeloMatrixEntry::Count; i++)
    scalar.entries[i] = array.entries[i].Last();
}

inline void
FBNoteKeyMatrixInitArrayFromScalar(
  FBNoteKeyMatrix<FBSArray<float, FBFixedBlockSamples>>& array,
  FBNoteKeyMatrix<float> const& scalar)
{
  for (int i = 0; i < (int)FBNoteKeyMatrixEntry::Count; i++)
    array.entries[i].Fill(scalar.entries[i]);
}

inline void
FBNoteKeyMatrixInitScalarFromArrayLast(
  FBNoteKeyMatrix<float>& scalar,
  FBNoteKeyMatrix<FBSArray<float, FBFixedBlockSamples>> const& array)
{
  for (int i = 0; i < (int)FBNoteKeyMatrixEntry::Count; i++)
    scalar.entries[i] = array.entries[i].Last();
}

inline void
FBNoteMatrixInitArrayFromScalar(
  FBNoteMatrix<FBSArray<float, FBFixedBlockSamples>>& array,
  FBNoteMatrix<float> const& scalar)
{
  FBNoteKeyMatrixInitArrayFromScalar(array.key, scalar.key);
  FBNoteVeloMatrixInitArrayFromScalar(array.velo, scalar.velo);
}

inline void
FBNoteMatrixInitScalarFromArrayLast(
  FBNoteMatrix<float>& scalar,
  FBNoteMatrix<FBSArray<float, FBFixedBlockSamples>> const& array)
{
  FBNoteKeyMatrixInitScalarFromArrayLast(scalar.key, array.key);
  FBNoteVeloMatrixInitScalarFromArrayLast(scalar.velo, array.velo);
}