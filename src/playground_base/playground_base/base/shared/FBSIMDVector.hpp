#pragma once

#include <immintrin.h> // todo neon

#define FB_SIMD_CALL __vectorcall
inline int constexpr FBSIMDVectorBitCount = 256;
inline int constexpr FBSIMDVectorByteCount = FBSIMDVectorBitCount / 8;
inline int constexpr FBSIMDVectorFloatCount = FBSIMDVectorByteCount / sizeof(float);

class alignas(FBSIMDVectorByteCount) FBSIMDFloatVector
{
  __m256 _store;
  explicit FBSIMDFloatVector(__m256 store);

public:
  FBSIMDFloatVector();

  float& operator[](int index);
  float operator[](int index) const;

  void FB_SIMD_CALL SetToZero();
  void FB_SIMD_CALL MultiplyByOneMinus(FBSIMDFloatVector rhs);
  void FB_SIMD_CALL FMA(FBSIMDFloatVector b, FBSIMDFloatVector c);
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
_store(_mm256_setzero_ps()) {}

inline FBSIMDFloatVector::
FBSIMDFloatVector(__m256 store) :
_store(store) {}

inline void
FBSIMDFloatVector::SetToZero()
{
  _store = _mm256_setzero_ps();
}

inline float& 
FBSIMDFloatVector::operator[](int index)
{
  return _store.m256_f32[index];
}

inline float 
FBSIMDFloatVector::operator[](int index) const
{
  return _store.m256_f32[index];
}

inline FBSIMDFloatVector& 
FBSIMDFloatVector::operator=(FBSIMDFloatVector rhs)
{
  _store = rhs._store;
  return *this;
}

inline FBSIMDFloatVector&
FBSIMDFloatVector::operator+=(FBSIMDFloatVector rhs)
{
  _store = _mm256_add_ps(_store, rhs._store);
  return *this;
}

inline FBSIMDFloatVector& 
FBSIMDFloatVector::operator-=(FBSIMDFloatVector rhs)
{
  _store = _mm256_sub_ps(_store, rhs._store);
  return *this;
}

inline FBSIMDFloatVector& 
FBSIMDFloatVector::operator*=(FBSIMDFloatVector rhs)
{
  _store = _mm256_mul_ps(_store, rhs._store);
  return *this;
}

inline FBSIMDFloatVector& 
FBSIMDFloatVector::operator/=(FBSIMDFloatVector rhs)
{
  _store = _mm256_div_ps(_store, rhs._store);
  return *this;
}

inline FBSIMDFloatVector 
FBSIMDFloatVector::operator+(FBSIMDFloatVector rhs) const
{
  return FBSIMDFloatVector(_mm256_add_ps(_store, rhs._store));
}

inline FBSIMDFloatVector 
FBSIMDFloatVector::operator-(FBSIMDFloatVector rhs) const
{
  return FBSIMDFloatVector(_mm256_sub_ps(_store, rhs._store));
}

inline FBSIMDFloatVector 
FBSIMDFloatVector::operator*(FBSIMDFloatVector rhs) const
{
  return FBSIMDFloatVector(_mm256_mul_ps(_store, rhs._store));
}

inline FBSIMDFloatVector 
FBSIMDFloatVector::operator/(FBSIMDFloatVector rhs) const
{
  return FBSIMDFloatVector(_mm256_div_ps(_store, rhs._store));
}

inline void
FBSIMDFloatVector::FMA(FBSIMDFloatVector b, FBSIMDFloatVector c)
{
  _store = _mm256_fmadd_ps(b._store, c._store, _store);
}

inline void
FBSIMDFloatVector::MultiplyByOneMinus(FBSIMDFloatVector rhs)
{
  __m256 one = _mm256_set1_ps(1.0f);
  __m256 oneMinus = _mm256_sub_ps(one, rhs._store);
  _store = _mm256_mul_ps(_store, oneMinus);
}
