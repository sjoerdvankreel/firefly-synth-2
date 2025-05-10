#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <array>
#include <xsimd/xsimd.hpp>

// better make sure this all lines up
typedef xsimd::sse2 FBXSIMDBatchType;
inline int constexpr FBSIMDFloatCount = 4;
inline int constexpr FBSIMDAlign = FBSIMDFloatCount * sizeof(float);
inline int constexpr FBFixedBlockSamples = 4 * FBSIMDFloatCount;

template <class T> using FBSIMDVector = xsimd::batch<T, xsimd::sse2>;
template <class T> struct FBSIMDTraits {
  static inline int constexpr Size = FBSIMDVector<T>::size; 
  static inline int constexpr Align = Size * sizeof(T); };

// todo drop the other ones
template <class T, int N>
class alignas(FBSIMDTraits<T>::Align) FBSIMDArray
{
  static_assert(N % FBSIMDTraits<T>::Size == 0);
  alignas(FBSIMDTraits<T>::Align) std::array<T, N> _data = {};

public:
  FBSIMDArray() = default;
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBSIMDArray);
  FBSIMDArray(FBSIMDVector<T> val) { Fill(val); }
  FBSIMDArray(T val) { Fill(FBSIMDVector<T>(val)); }

  void NaNCheck() const;
  template <int Times> void UpsampleStretch();

  T Last() const { return _data[N - 1]; }
  T Get(int pos) const { return _data[pos]; };
  void Set(int pos, T val) { _data[pos] = val; };
  T* Ptr(int offset) { return &_data[offset]; }
  T const* Ptr(int offset) const { return &_data[offset]; }

  void Store(int pos, FBSIMDVector<T> val) { val.store_aligned(Ptr(pos)); }
  void Add(int pos, FBSIMDVector<T> val) { (Load(pos) + val).store_aligned(Ptr(pos)); }
  void Mul(int pos, FBSIMDVector<T> val) { (Load(pos) * val).store_aligned(Ptr(pos)); }
  FBSIMDVector<T> Load(int pos) const { return FBSIMDVector<T>::load_aligned(Ptr(pos)); }

  void Fill(FBSIMDVector<T> val) { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) Store(i, val); }
  void Mul(FBSIMDArray<T, N> const& rhs) { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) Mul(i, rhs.Load(i)); }
  void Add(FBSIMDArray<T, N> const& rhs) { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) Add(i, rhs.Load(i)); }
  void CopyTo(FBSIMDArray<T, N>& rhs) const { for (int i = 0; i < N; i += FBSIMDTraits<T>::Size) rhs.Store(i, Load(i)); }
};

template <class T, int N>
inline void
FBSIMDArray<T, N>::NaNCheck() const
{
#ifndef NDEBUG
  for (int i = 0; i < N; i++)
    assert(!std::isnan(Get(i)));
#endif
}

template <class T, int N>
template <int Times>
inline void 
FBSIMDArray<T, N>::UpsampleStretch()
{
  // need temp for perf opt
  FBSIMDArray<T, N / Times> x;
  static_assert(Times == FBSIMDTraits<T>::Size);
  for (int s = 0; s < N / Times; s += FBSIMDTraits<T>::Size)
    x.Store(s, Load(s));
  for (int s = 0; s < N / Times; s++)
    Store(s * FBSIMDTraits<T>::Size, x.Get(s));
}

template <class T>
inline void
FBSIMDVectorNaNCheck(FBSIMDVector<T> vec)
{
#ifndef NDEBUG
  FBSIMDArray<float, FBSIMDTraits<T>::Size> check;
  check.Store(0, vec);
  check.NaNCheck();
#endif
}

template <class T, int N1, int N2>
class alignas(FBSIMDTraits<T>::Align) FBSIMDArray2
{
  alignas(FBSIMDTraits<T>::Align) std::array<FBSIMDArray<T, N1>, N2> _data = {};

public:
  FBSIMDArray<T, N1>& operator[](int i) { return _data[i]; }
  FBSIMDArray<T, N1> const& operator[](int i) const { return _data[i]; }
  void NaNCheck() const { for (int i = 0; i < N2; i++) _data[i].NaNCheck(); }
  void Fill(FBSIMDVector<T> val) { for (int i = 0; i < N2; i++) _data[i].Fill(val); }
  void Mul(FBSIMDArray<T, N1> const& rhs) { for (int i = 0; i < N2; i++) _data[i].Mul(rhs); }
  void Add(FBSIMDArray2<T, N1, N2> const& rhs) { for (int i = 0; i < N2; i++) _data[i].Add(rhs._data[i]); }
  void Mul(FBSIMDArray2<T, N1, N2> const& rhs) { for (int i = 0; i < N2; i++) _data[i].Mul(rhs._data[i]); }
  void CopyTo(FBSIMDArray2<T, N1, N2>& rhs) const { for (int i = 0; i < N2; i++) _data[i].CopyTo(rhs._data[i]); }
};

template <class T, int N1, int N2, int N3>
class alignas(FBSIMDTraits<T>::Align) FBSIMDArray3
{
  alignas(FBSIMDTraits<T>::Align) std::array<FBSIMDArray2<T, N1, N2>, N3> _data = {};
public:
  FBSIMDArray2<T, N1, N2>& operator[](int i) { return _data[i]; }
  FBSIMDArray2<T, N1, N2> const& operator[](int i) const { return _data[i]; }
  void NaNCheck() const { for (int i = 0; i < N3; i++) _data[i].NaNCheck(); }
  void Fill(FBSIMDVector<T> val) { for (int i = 0; i < N3; i++) _data[i].Fill(val); }
  void Mul(FBSIMDArray<T, N1> const& rhs) { for (int i = 0; i < N3; i++) _data[i].Mul(rhs); }
  void Add(FBSIMDArray3<T, N1, N2, N3> const& rhs) { for (int i = 0; i < N3; i++) _data[i].Add(rhs._data[i]); }
  void Mul(FBSIMDArray3<T, N1, N2, N3> const& rhs) { for (int i = 0; i < N3; i++) _data[i].Mul(rhs._data[i]); }
  void CopyTo(FBSIMDArray3<T, N1, N2, N3>& rhs) const { for (int i = 0; i < N3; i++) _data[i].CopyTo(rhs._data[i]); }
};