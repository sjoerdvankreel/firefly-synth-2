#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <cstdlib>
#include <xsimd/xsimd.hpp>

template <class T>
class FBDArray
{
  T* _data = {};
  int _count = {};
  void BoundsCheck(int pos) const { assert(0 <= pos && pos < _count); }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBDArray);
  FBDArray() : _data(nullptr), _count(0) {}
  ~FBDArray() { FBAlignedFree(_data); _data = nullptr; _count = 0; }

  void Resize(int count);
  int Count() const { return _count; }
  T Get(int pos) const { BoundsCheck(pos); return _data[pos]; };
  void Set(int pos, T val) { BoundsCheck(pos); _data[pos] = val; };
  void Store(int pos, FBBatch<T> val) { val.store_aligned(Ptr(pos)); }
  FBBatch<T> Load(int pos) const { return FBBatch<T>::load_aligned(Ptr(pos)); }
  T* Ptr(int offset) { BoundsCheck(offset); return &_data[offset]; }
  T const* Ptr(int offset) const { BoundsCheck(offset); return &_data[offset]; }
};

template <class T>
inline void
FBDArray<T>::Resize(int count)
{
  FBAlignedFree(_data); 
  _count = count; 
  _data = static_cast<T*>(FBAlignedAlloc(FBSIMDAlign, count * sizeof(T)));
  assert(count % FBSIMDTraits<T>::Size == 0);
}