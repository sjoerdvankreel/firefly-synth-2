#pragma once

#if FB_SIMD_VECTOR_USE_AVX
#include <immintrin.h>
typedef __m256 FBSIMDType;
#define FB_SIMD_FLOAT_VECTOR_SIZE 8
#define FB_SIMD_VECTOR_CALL __vectorcall

#define FBSIMDFloatVectorAdd _mm256_add_ps
#define FBSIMDFloatVectorSub _mm256_sub_ps
#define FBSIMDFloatVectorMul _mm256_mul_ps
#define FBSIMDFloatVectorDiv _mm256_div_ps
#define FBSIMDFloatVectorInit _mm256_setzero_ps
#else
#error
#endif

class alignas(FB_SIMD_FLOAT_VECTOR_SIZE * sizeof(float)) FBSIMDFloatVector
{
  FBSIMDType _store;
  explicit FBSIMDFloatVector(FBSIMDType store);

public:
  FBSIMDFloatVector();

  FBSIMDFloatVector FB_SIMD_VECTOR_CALL operator+(FBSIMDFloatVector rhs);
  FBSIMDFloatVector FB_SIMD_VECTOR_CALL operator-(FBSIMDFloatVector rhs);
  FBSIMDFloatVector FB_SIMD_VECTOR_CALL operator*(FBSIMDFloatVector rhs);
  FBSIMDFloatVector FB_SIMD_VECTOR_CALL operator/(FBSIMDFloatVector rhs);

  FBSIMDFloatVector& FB_SIMD_VECTOR_CALL operator+=(FBSIMDFloatVector rhs);
  FBSIMDFloatVector& FB_SIMD_VECTOR_CALL operator-=(FBSIMDFloatVector rhs);
  FBSIMDFloatVector& FB_SIMD_VECTOR_CALL operator*=(FBSIMDFloatVector rhs);
  FBSIMDFloatVector& FB_SIMD_VECTOR_CALL operator/=(FBSIMDFloatVector rhs);
};

inline 
FBSIMDFloatVector::
FBSIMDFloatVector():
_store(FBSIMDFloatVectorInit()) {}

inline
FBSIMDFloatVector::
FBSIMDFloatVector(FBSIMDType store) :
_store(store) {}

inline FBSIMDFloatVector&
FBSIMDFloatVector::operator+=(FBSIMDFloatVector rhs)
{ _store = FBSIMDFloatVectorAdd(_store, rhs._store); return *this; }
inline FBSIMDFloatVector&
FBSIMDFloatVector::operator-=(FBSIMDFloatVector rhs)
{ _store = FBSIMDFloatVectorSub(_store, rhs._store); return *this; }
inline FBSIMDFloatVector&
FBSIMDFloatVector::operator*=(FBSIMDFloatVector rhs)
{ _store = FBSIMDFloatVectorMul(_store, rhs._store); return *this; }
inline FBSIMDFloatVector&
FBSIMDFloatVector::operator/=(FBSIMDFloatVector rhs)
{ _store = FBSIMDFloatVectorDiv(_store, rhs._store); return *this; }

inline FBSIMDFloatVector
FBSIMDFloatVector::operator+(FBSIMDFloatVector rhs)
{ return FBSIMDFloatVector(FBSIMDFloatVectorAdd(_store, rhs._store)); }
inline FBSIMDFloatVector
FBSIMDFloatVector::operator-(FBSIMDFloatVector rhs)
{ return FBSIMDFloatVector(FBSIMDFloatVectorSub(_store, rhs._store)); }
inline FBSIMDFloatVector
FBSIMDFloatVector::operator*(FBSIMDFloatVector rhs)
{ return FBSIMDFloatVector(FBSIMDFloatVectorMul(_store, rhs._store)); }
inline FBSIMDFloatVector
FBSIMDFloatVector::operator/(FBSIMDFloatVector rhs)
{ return FBSIMDFloatVector(FBSIMDFloatVectorDiv(_store, rhs._store)); }