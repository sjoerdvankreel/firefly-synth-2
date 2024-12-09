#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBSIMDVector.hpp>

#include <array>
#include <cassert>

class alignas(FB_SIMD_VECTOR_ALIGN)
FBFixedCVBlock final
{
  std::array<float, FB_FIXED_BLOCK_SIZE> _store = {};

public:
  void Fill(int from, int to, float val);
  void CopyFrom(FBFixedCVBlock const& rhs);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedCVBlock);
  
  static int Count()
  { return FB_FIXED_BLOCK_SIZE; }
  float& operator[](int sample)
  { return _store[sample]; }
  float const& operator[](int sample) const
  { return _store[sample]; }
};

inline void
FBFixedCVBlock::Fill(int from, int to, float val)
{
  assert(0 <= from && from <= to && to <= Count());
  std::fill(_store.begin() + from, _store.begin() + to, val);
}

inline void 
FBFixedCVBlock::CopyFrom(FBFixedCVBlock const& rhs)
{
  for (int i = 0; i < Count(); i++)
    (*this)[i] = rhs[i];
}