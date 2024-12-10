#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <cassert>

class alignas(FBFixedBlockAlign)
FBFixedCVBlock final
{
  std::array<float, FBFixedBlockSize> _store = {};

public:
  void Fill(int from, int to, float val);
  void CopyFrom(FBFixedCVBlock const& rhs);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedCVBlock);
  
  static int Count() { return FBFixedBlockSize; }
  float& operator[](int sample) { return _store[sample]; }
  float const& operator[](int sample) const { return _store[sample]; }
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