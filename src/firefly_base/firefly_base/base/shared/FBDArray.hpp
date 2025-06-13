#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <cstdlib>
#include <xsimd/xsimd.hpp>

template <class T>
class FBDArray final
{
  int _count = 0;
  T* _data = nullptr;
  void BoundsCheck(int pos) const { FB_ASSERT(0 <= pos && pos < _count); }

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBDArray);
  FBDArray() : _count(0), _data(nullptr) {}
  ~FBDArray() { FBAlignedFree(_data); _data = nullptr; _count = 0; }

  void Resize(int count);
  int Count() const { return _count; }
  void SetToZero() { std::memset(_data, 0, _count * sizeof(T)); }
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
  FB_ASSERT(count % FBSIMDTraits<T>::Size == 0);
  if (_count != count)
  {
    FBAlignedFree(_data);
    _count = count;
    _data = static_cast<T*>(FBAlignedAlloc(FBSIMDAlign, count * sizeof(T)));
  }  
}