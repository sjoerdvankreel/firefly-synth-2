#pragma once

#include <numbers>

// todo neon
#if FB_USE_SSE
#if FB_USE_AVX
#error
#endif
#include <immintrin.h>

typedef __m128 FBVectorStore;
inline int constexpr FBVectorBitCount = 128;

#define FBVectorCall __vectorcall
#define FBVectorFloatAddr(x) (x.m128_f32)

#define FBVectorFloatAdd _mm_add_ps
#define FBVectorFloatSub _mm_sub_ps
#define FBVectorFloatMul _mm_mul_ps
#define FBVectorFloatDiv _mm_div_ps
#define FBVectorFloatSin _mm_sin_ps
#define FBVectorFloatFMA _mm_fmadd_ps
#define FBVectorFloatSet1 _mm_set1_ps
#define FBVectorFloatSetZero _mm_setzero_ps

#elif FB_USE_AVX
#include <immintrin.h>

typedef __m256 FBVectorStore;
inline int constexpr FBVectorBitCount = 256;

#define FBVectorCall __vectorcall
#define FBVectorFloatAddr(x) (x.m256_f32)

#define FBVectorFloatAdd _mm256_add_ps
#define FBVectorFloatSub _mm256_sub_ps
#define FBVectorFloatMul _mm256_mul_ps
#define FBVectorFloatDiv _mm256_div_ps
#define FBVectorFloatSin _mm256_sin_ps
#define FBVectorFloatFMA _mm256_fmadd_ps
#define FBVectorFloatSet1 _mm256_set1_ps
#define FBVectorFloatSetZero _mm256_setzero_ps

#else
#error
#endif

inline int constexpr FBVectorByteCount = FBVectorBitCount / 8;
inline int constexpr FBVectorFloatCount = FBVectorByteCount / sizeof(float);

class alignas(FBVectorByteCount) FBFloatVector
{
  FBVectorStore _store;
  FBFloatVector(FBVectorStore store) : _store(store) {}

public:
  FBFloatVector() : _store(FBVectorFloatSetZero()) {}
  FBFloatVector(float x) : _store(FBVectorFloatSet1(x)) {}

  void FBVectorCall Clear() { *this = 0.0f; }
  float& operator[](int index) { return FBVectorFloatAddr(_store)[index]; }
  float operator[](int index) const { return FBVectorFloatAddr(_store)[index]; }

  friend FBFloatVector operator+(float l, FBFloatVector r);
  friend FBFloatVector operator-(float l, FBFloatVector r);
  friend FBFloatVector operator*(float l, FBFloatVector r);
  friend FBFloatVector operator/(float l, FBFloatVector r);
  friend FBFloatVector operator+(FBFloatVector l, float r);
  friend FBFloatVector operator-(FBFloatVector l, float r);
  friend FBFloatVector operator*(FBFloatVector l, float r);
  friend FBFloatVector operator/(FBFloatVector l, float r);
  friend FBFloatVector operator+(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector operator-(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector operator*(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector operator/(FBFloatVector l, FBFloatVector r);

  FBFloatVector& FBVectorCall operator+=(float rhs) { return *this = *this + rhs; }
  FBFloatVector& FBVectorCall operator-=(float rhs) { return *this = *this - rhs; }
  FBFloatVector& FBVectorCall operator*=(float rhs) { return *this = *this * rhs; }
  FBFloatVector& FBVectorCall operator/=(float rhs) { return *this = *this / rhs; }
  FBFloatVector& FBVectorCall operator+=(FBFloatVector rhs) { return *this = *this + rhs; }
  FBFloatVector& FBVectorCall operator-=(FBFloatVector rhs) { return *this = *this - rhs; }
  FBFloatVector& FBVectorCall operator*=(FBFloatVector rhs) { return *this = *this * rhs; }
  FBFloatVector& FBVectorCall operator/=(FBFloatVector rhs) { return *this = *this / rhs; }

  FBFloatVector FBVectorCall Sin() { return FBVectorFloatSin(_store); }
  FBFloatVector FBVectorCall Unipolar() { return (*this + 1.0f) * 0.5f; }
  FBFloatVector& FBVectorCall operator=(FBFloatVector rhs) { _store = rhs._store; return *this; }
  FBFloatVector& FBVectorCall operator=(float rhs) { _store = FBVectorFloatSet1(rhs); return *this; }
};

inline FBFloatVector
operator+(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatAdd(l._store, r._store); }
inline FBFloatVector
operator-(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatSub(l._store, r._store); }
inline FBFloatVector
operator*(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatMul(l._store, r._store); }
inline FBFloatVector
operator/(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatDiv(l._store, r._store); }

inline FBFloatVector
operator+(float l, FBFloatVector r)
{ return FBVectorFloatAdd(FBVectorFloatSet1(l), r._store); }
inline FBFloatVector
operator-(float l, FBFloatVector r)
{ return FBVectorFloatSub(FBVectorFloatSet1(l), r._store); }
inline FBFloatVector
operator*(float l, FBFloatVector r)
{ return FBVectorFloatMul(FBVectorFloatSet1(l), r._store); }
inline FBFloatVector
operator/(float l, FBFloatVector r)
{ return FBVectorFloatDiv(FBVectorFloatSet1(l), r._store); }

inline FBFloatVector
operator+(FBFloatVector l, float r)
{ return FBVectorFloatAdd(l._store, FBVectorFloatSet1(r)); }
inline FBFloatVector
operator-(FBFloatVector l, float r)
{ return FBVectorFloatSub(l._store, FBVectorFloatSet1(r)); }
inline FBFloatVector
operator*(FBFloatVector l, float r)
{ return FBVectorFloatMul(l._store, FBVectorFloatSet1(r)); }
inline FBFloatVector
operator/(FBFloatVector l, float r)
{ return FBVectorFloatDiv(l._store, FBVectorFloatSet1(r)); }
