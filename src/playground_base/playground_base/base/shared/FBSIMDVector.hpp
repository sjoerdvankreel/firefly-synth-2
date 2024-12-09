#pragma once

#if FB_SIMD_VECTOR_USE_AVX
#include <immintrin.h>
typedef __m256 FBSIMDType;
#define FB_SIMD_VECTOR_CALL __vectorcall

#define FBSIMDFloatVectorAdd _mm256_add_ps
#define FBSIMDFloatVectorSub _mm256_sub_ps
#define FBSIMDFloatVectorMul _mm256_mul_ps
#define FBSIMDFloatVectorDiv _mm256_div_ps
#define FBSIMDFloatVectorInit _mm256_setzero_ps
#define FBSIMDFloatVectorAddr(x, i) (x.m256_f32[i])

inline int constexpr FBSIMDVectorBitWidth = 256;
inline int constexpr FBSIMDVectorByteWidth = FBSIMDVectorBitWidth / 8;
inline int constexpr FBSIMDVectorAlign = FBSIMDVectorByteWidth;
inline int constexpr FBSIMDVectorFloatCount = FBSIMDVectorByteWidth / sizeof(float);
#else
#error
#endif

class alignas(FBSIMDVectorAlign) FBSIMDFloatVector
{
  FBSIMDType _store;
  explicit FBSIMDFloatVector(FBSIMDType store);

public:
  FBSIMDFloatVector();

  void SetToZero();
  float& operator[](int i);
  float operator[](int i) const;

  void FB_SIMD_VECTOR_CALL operator+=(FBSIMDFloatVector rhs);
  void FB_SIMD_VECTOR_CALL operator-=(FBSIMDFloatVector rhs);
  void FB_SIMD_VECTOR_CALL operator*=(FBSIMDFloatVector rhs);
  void FB_SIMD_VECTOR_CALL operator/=(FBSIMDFloatVector rhs);
};

inline 
FBSIMDFloatVector::
FBSIMDFloatVector():
_store(FBSIMDFloatVectorInit()) {}

inline
FBSIMDFloatVector::
FBSIMDFloatVector(FBSIMDType store) :
_store(store) {}

inline void
FBSIMDFloatVector::SetToZero()
{ _store = FBSIMDFloatVectorInit(); }

inline float& 
FBSIMDFloatVector::operator[](int i)
{ return FBSIMDFloatVectorAddr(_store, i); }
inline float 
FBSIMDFloatVector::operator[](int i) const
{ return FBSIMDFloatVectorAddr(_store, i); }

inline void
FBSIMDFloatVector::operator+=(FBSIMDFloatVector rhs)
{ _store = FBSIMDFloatVectorAdd(_store, rhs._store); }
inline void
FBSIMDFloatVector::operator-=(FBSIMDFloatVector rhs)
{ _store = FBSIMDFloatVectorSub(_store, rhs._store); }
inline void
FBSIMDFloatVector::operator*=(FBSIMDFloatVector rhs)
{ _store = FBSIMDFloatVectorMul(_store, rhs._store); }
inline void
FBSIMDFloatVector::operator/=(FBSIMDFloatVector rhs)
{ _store = FBSIMDFloatVectorDiv(_store, rhs._store); }