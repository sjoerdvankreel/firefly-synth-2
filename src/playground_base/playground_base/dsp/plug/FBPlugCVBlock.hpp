#pragma once

#include <playground_base/base/plug/FBPlugConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <cassert>

class alignas(FB_PLUG_BLOCK_ALIGN)
FBPlugCVBlock
{
  std::array<float, FB_PLUG_BLOCK_SIZE> _store = {};

public:
  void Fill(int from, int to, float val);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugCVBlock);
  
  static int Count()
  { return FB_PLUG_BLOCK_SIZE; }
  float& operator[](int sample)
  { return _store[sample]; }
  float const& operator[](int sample) const
  { return _store[sample]; }
};

inline void
FBPlugCVBlock::Fill(int from, int to, float val)
{
  assert(0 <= from && from <= to && to <= Count());
  std::fill(_store.begin() + from, _store.begin() + to, val);
}