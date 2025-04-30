#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <array>
#include <xsimd/xsimd.hpp>

template <class T> using FBSIMDVector = xsimd::batch<T, xsimd::sse2>;
template <class T> struct FBSIMDTraits {
  static inline int constexpr Size = FBSIMDVector<T>::size; 
  static inline int constexpr Align = Size * sizeof(T); };

// todo drop the other ones
template <class T, int N>
class alignas(FBSIMDTraits<T>::Align) FBSIMDArray
{
  static_assert(N% FBSIMDTraits<T>::Size == 0);
  alignas(FBSIMDTraits<T>::Align) std::array<T, N> _data = {};

public:
  FBSIMDArray() = default;
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBSIMDArray);
  FBSIMDArray(FBSIMDVector<T> val) { Fill(val); }
  FBSIMDArray(T val) { Fill(FBSIMDVector<T>(val)); }

  void UpsampleStretch(int times);
  static int UpsampleOffset(int times) { return (times - 1) * (N / times); }

  T Last() const { return _data[N - 1]; }
  T Get(int pos) const { return _data[pos]; };
  void Set(int pos, T val) { _data[pos] = val; };
  T* Ptr(int offset) { return &_data[offset]; }
  T const* Ptr(int offset) const { return &_data[offset]; }
  void Store(int pos, FBSIMDVector<T> val) { val.store_aligned(Ptr(pos)); }
  void Add(int pos, FBSIMDVector<T> val) { (Load(pos) + val).store_aligned(Ptr(pos)); }
  FBSIMDVector<T> Load(int pos) const { return FBSIMDVector<T>::load_aligned(Ptr(pos)); }
  void Fill(FBSIMDVector<T> val) { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) Store(i, val); }
};

template <class T, int N>
inline void 
FBSIMDArray<T, N>::UpsampleStretch(int times)
{
  int offset = UpsampleOffset(times);
  for (int s = 0; s < N; s++)
    Set(s, Get(offset + s / times));
}

template <class T, int N1, int N2>
class alignas(FBSIMDTraits<T>::Align) FBSIMDArray2
{
  alignas(FBSIMDTraits<T>::Align) std::array<FBSIMDArray<T, N1>, N2> _data = {};
public:
  FBSIMDArray<T, N1>& operator[](int i) { return _data[i]; }
  FBSIMDArray<T, N1> const& operator[](int i) const { return _data[i]; }
  void Fill(FBSIMDVector<T> val) { for (int i = 0; i < N2; i++) _data[i].Fill(val); }
};

template <class T, int N1, int N2, int N3>
class alignas(FBSIMDTraits<T>::Align) FBSIMDArray3
{
  alignas(FBSIMDTraits<T>::Align) std::array<FBSIMDArray2<T, N1, N2>, N3> _data = {};
public:
  FBSIMDArray2<T, N1, N2>& operator[](int i) { return _data[i]; }
  FBSIMDArray2<T, N1, N2> const& operator[](int i) const { return _data[i]; }
  void Fill(FBSIMDVector<T> val) { for (int i = 0; i < N3; i++) _data[i].Fill(val); }
};