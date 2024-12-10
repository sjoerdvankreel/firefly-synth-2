#pragma once

#include <numbers>

// todo neon
#if FB_USE_AVX

#include <immintrin.h>
typedef __m256 FBSIMDFloatStore;
inline int constexpr FBSIMDVectorBitCount = 256;
#define FB_SIMD_CALL __vectorcall

#define FBSIMDFloatAdd _mm256_add_ps
#define FBSIMDFloatSub _mm256_sub_ps
#define FBSIMDFloatMul _mm256_mul_ps
#define FBSIMDFloatDiv _mm256_div_ps
#define FBSIMDFloatSin _mm256_sin_ps
#define FBSIMDFloatFMA _mm256_fmadd_ps
#define FBSIMDFloatSet1 _mm256_set1_ps
#define FBSIMDFloatZero _mm256_setzero_ps

#else
#error
#endif

inline int constexpr FBSIMDVectorByteCount = 
FBSIMDVectorBitCount / 8;
inline int constexpr FBSIMDVectorFloatCount = 
FBSIMDVectorByteCount / sizeof(float);

class alignas(FBSIMDVectorByteCount) FBSIMDFloatVector
{
  FBSIMDFloatStore _store;
  explicit FBSIMDFloatVector(FBSIMDFloatStore store);

public:
  FBSIMDFloatVector();

  float& operator[](int index);
  float operator[](int index) const;

  void FB_SIMD_CALL SetToZero();
  void FB_SIMD_CALL SetToSineOfTwoPi();
  void FB_SIMD_CALL SetToUnipolarSineOfTwoPi();
  void FB_SIMD_CALL MultiplyByOneMinus(FBSIMDFloatVector rhs);
  void FB_SIMD_CALL FMA1(FBSIMDFloatVector b, FBSIMDFloatVector c);
  void FB_SIMD_CALL FMA2(FBSIMDFloatVector b, FBSIMDFloatVector c, FBSIMDFloatVector d);

  FBSIMDFloatVector& FB_SIMD_CALL operator=(FBSIMDFloatVector rhs);

  FBSIMDFloatVector& FB_SIMD_CALL operator+=(FBSIMDFloatVector rhs);
  FBSIMDFloatVector& FB_SIMD_CALL operator-=(FBSIMDFloatVector rhs);
  FBSIMDFloatVector& FB_SIMD_CALL operator*=(FBSIMDFloatVector rhs);
  FBSIMDFloatVector& FB_SIMD_CALL operator/=(FBSIMDFloatVector rhs);

  FBSIMDFloatVector FB_SIMD_CALL operator+(FBSIMDFloatVector rhs) const;
  FBSIMDFloatVector FB_SIMD_CALL operator-(FBSIMDFloatVector rhs) const;
  FBSIMDFloatVector FB_SIMD_CALL operator*(FBSIMDFloatVector rhs) const;
  FBSIMDFloatVector FB_SIMD_CALL operator/(FBSIMDFloatVector rhs) const;
};

inline FBSIMDFloatVector::
FBSIMDFloatVector() :
_store(FBSIMDFloatZero()) {}

inline FBSIMDFloatVector::
FBSIMDFloatVector(FBSIMDFloatStore store) :
_store(store) {}

inline float& 
FBSIMDFloatVector::operator[](int index)
{ return _store.m256_f32[index]; }

inline float 
FBSIMDFloatVector::operator[](int index) const
{ return _store.m256_f32[index]; }

inline FBSIMDFloatVector& 
FBSIMDFloatVector::operator=(FBSIMDFloatVector rhs)
{ _store = rhs._store; return *this; }

inline FBSIMDFloatVector&
FBSIMDFloatVector::operator+=(FBSIMDFloatVector rhs)
{
  _store = FBSIMDFloatAdd(_store, rhs._store); return *this;
}

inline FBSIMDFloatVector& 
FBSIMDFloatVector::operator-=(FBSIMDFloatVector rhs)
{
  _store = FBSIMDFloatSub(_store, rhs._store);
  return *this;
}

inline FBSIMDFloatVector& 
FBSIMDFloatVector::operator*=(FBSIMDFloatVector rhs)
{
  _store = FBSIMDFloatMul(_store, rhs._store);
  return *this;
}

inline FBSIMDFloatVector& 
FBSIMDFloatVector::operator/=(FBSIMDFloatVector rhs)
{
  _store = FBSIMDFloatDiv(_store, rhs._store);
  return *this;
}

inline FBSIMDFloatVector 
FBSIMDFloatVector::operator+(FBSIMDFloatVector rhs) const
{
  return FBSIMDFloatVector(FBSIMDFloatAdd(_store, rhs._store));
}

inline FBSIMDFloatVector 
FBSIMDFloatVector::operator-(FBSIMDFloatVector rhs) const
{
  return FBSIMDFloatVector(FBSIMDFloatSub(_store, rhs._store));
}

inline FBSIMDFloatVector 
FBSIMDFloatVector::operator*(FBSIMDFloatVector rhs) const
{
  return FBSIMDFloatVector(FBSIMDFloatMul(_store, rhs._store));
}

inline FBSIMDFloatVector 
FBSIMDFloatVector::operator/(FBSIMDFloatVector rhs) const
{
  return FBSIMDFloatVector(FBSIMDFloatDiv(_store, rhs._store));
}

inline void
FBSIMDFloatVector::SetToZero()
{
  _store = FBSIMDFloatZero();
}

inline void
FBSIMDFloatVector::SetToSineOfTwoPi()
{
  FBSIMDFloatStore twoPi = FBSIMDFloatSet1(2.0f * std::numbers::pi_v<float>);
  _store = FBSIMDFloatSin(FBSIMDFloatMul(_store, twoPi));
}

inline void
FBSIMDFloatVector::SetToUnipolarSineOfTwoPi()
{
  SetToSineOfTwoPi();
  FBSIMDFloatStore zeroTwo = FBSIMDFloatAdd(_store, FBSIMDFloatSet1(1.0f));
  _store = FBSIMDFloatMul(FBSIMDFloatSet1(0.5f), zeroTwo);
}

inline void
FBSIMDFloatVector::MultiplyByOneMinus(FBSIMDFloatVector rhs)
{
  FBSIMDFloatStore one = FBSIMDFloatSet1(1.0f);
  FBSIMDFloatStore oneMinus = FBSIMDFloatSub(one, rhs._store);
  _store = FBSIMDFloatMul(_store, oneMinus);
}

inline void
FBSIMDFloatVector::FMA1(FBSIMDFloatVector b, FBSIMDFloatVector c)
{
  _store = FBSIMDFloatFMA(b._store, c._store, _store);
}

inline void
FBSIMDFloatVector::FMA2(FBSIMDFloatVector b, FBSIMDFloatVector c, FBSIMDFloatVector d)
{
  _store = FBSIMDFloatFMA(FBSIMDFloatMul(b._store, c._store), d._store, _store);
}
