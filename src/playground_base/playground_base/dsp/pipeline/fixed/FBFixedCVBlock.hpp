#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <cassert>

class alignas(FB_FIXED_BLOCK_ALIGN)
FBFixedCVBlock
{
  std::array<float, FB_FIXED_BLOCK_SIZE> _store = {};

public:
  void Fill(int from, int to, float val);
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