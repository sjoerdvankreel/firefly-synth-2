#pragma once

#if FB_SIMD_VECTOR_USE_AVX
#include <immintrin.h>
typedef __m256 FBSIMDType;
#define FB_SIMD_FLOAT_VECTOR_SIZE 8
#define FB_SIMD_VECTOR_CALL __vectorcall
#define FBSIMDVectorInit _mm256_setzero_ps
#else
#error
#endif

class alignas(FB_SIMD_FLOAT_VECTOR_SIZE * sizeof(float)) FBSIMDFloatVector
{
  FBSIMDType _store = _mm256_setzero_ps();

  void boo()
  {
    _store.m256_f32[0] = 3.0f;
  }
};