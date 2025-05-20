#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <xsimd/xsimd.hpp>

template <class T, int N>
class alignas(FBSIMDTraits<T>::Align) FBArray
{
  static_assert(N % FBSIMDTraits<T>::Size == 0);
  alignas(FBSIMDTraits<T>::Align) std::array<T, N> _data = {};

public:
  FBArray() = default;
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBArray);
  FBArray(FBBatch<T> val) { Fill(val); }
  FBArray(T val) { Fill(FBBatch<T>(val)); }

  void NaNCheck() const;
  template <int Times> void UpsampleStretch();

  T First() const { return _data[0]; }
  T Last() const { return _data[N - 1]; }
  T Get(int pos) const { return _data[pos]; };
  void Set(int pos, T val) { _data[pos] = val; };
  T* Ptr(int offset) { return &_data[offset]; }
  T const* Ptr(int offset) const { return &_data[offset]; }

  FBBatch<double> LoadFloatToDouble(int pos) const;
  void StoreDoubleToFloat(int pos, FBBatch<double> val)  const;

  void Store(int pos, FBBatch<T> val) { val.store_aligned(Ptr(pos)); }
  void Add(int pos, FBBatch<T> val) { (Load(pos) + val).store_aligned(Ptr(pos)); }
  void Mul(int pos, FBBatch<T> val) { (Load(pos) * val).store_aligned(Ptr(pos)); }
  FBBatch<T> Load(int pos) const { return FBBatch<T>::load_aligned(Ptr(pos)); }

  void Fill(FBBatch<T> val) { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) Store(i, val); }
  void Mul(FBArray<T, N> const& rhs) { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) Mul(i, rhs.Load(i)); }
  void Add(FBArray<T, N> const& rhs) { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) Add(i, rhs.Load(i)); }
  void CopyTo(FBArray<T, N>& rhs) const { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) rhs.Store(i, Load(i)); }
};

template <class T, int N>
inline FBBatch<double>
FBArray<T, N>::LoadFloatToDouble(int pos) const
{
  static_assert(std::is_same<T, float>::value);
  assert(pos % FBSIMDTraits<double>::Size == 0);
  FBArray<double, FBSIMDTraits<double>::Size> y;
  for (int i = 0; i < FBSIMDTraits<double>::Size; i++)
    y.Set(i, Get(pos + i));
  return y.Load(0);
}

template <class T, int N>
inline void 
FBArray<T, N>::StoreDoubleToFloat(int pos, FBBatch<double> val)  const
{
  static_assert(std::is_same<T, float>::value);
  assert(pos % FBSIMDTraits<double>::Size == 0);
  FBArray<double, FBSIMDTraits<double>::Size> x(val);
  for (int i = 0; i < FBSIMDTraits<double>::Size; i++)
    Set(pos + i, static_cast<float>(x.Get(i)));
}

template <class T, int N>
inline void
FBArray<T, N>::NaNCheck() const
{
#ifndef NDEBUG
  for (int i = 0; i < N; i++)
    assert(!std::isnan(Get(i)));
#endif
}

template <class T, int N>
template <int Times>
inline void 
FBArray<T, N>::UpsampleStretch()
{
  // need temp for perf opt
  FBArray<T, N / Times> x;
  static_assert(Times == FBSIMDTraits<T>::Size);
  for (int s = 0; s < N / Times; s += FBSIMDTraits<T>::Size)
    x.Store(s, Load(s));
  for (int s = 0; s < N / Times; s++)
    Store(s * FBSIMDTraits<T>::Size, x.Get(s));
}

template <class T>
inline void
FBBatchNaNCheck(FBBatch<T> vec)
{
#ifndef NDEBUG
  FBArray<float, FBSIMDTraits<T>::Size> check;
  check.Store(0, vec);
  check.NaNCheck();
#endif
}

template <class T, int N1, int N2>
class alignas(FBSIMDTraits<T>::Align) FBArray2
{
  alignas(FBSIMDTraits<T>::Align) std::array<FBArray<T, N1>, N2> _data = {};
public:
  FBArray<T, N1>& operator[](int i) { return _data[i]; }
  FBArray<T, N1> const& operator[](int i) const { return _data[i]; }
  void NaNCheck() const { for (int i = 0; i < N2; i++) _data[i].NaNCheck(); }
  void Fill(FBBatch<T> val) { for (int i = 0; i < N2; i++) _data[i].Fill(val); }
  void Mul(FBArray<T, N1> const& rhs) { for (int i = 0; i < N2; i++) _data[i].Mul(rhs); }
  void Add(FBArray2<T, N1, N2> const& rhs) { for (int i = 0; i < N2; i++) _data[i].Add(rhs._data[i]); }
  void Mul(FBArray2<T, N1, N2> const& rhs) { for (int i = 0; i < N2; i++) _data[i].Mul(rhs._data[i]); }
  void CopyTo(FBArray2<T, N1, N2>& rhs) const { for (int i = 0; i < N2; i++) _data[i].CopyTo(rhs._data[i]); }
};