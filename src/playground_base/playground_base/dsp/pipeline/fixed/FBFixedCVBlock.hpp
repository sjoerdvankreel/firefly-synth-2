#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedSIMDBlock.hpp>

#include <array>
#include <cassert>

class alignas(FBSIMDVectorAlign)
FBFixedCVBlock final
{
  FBFixedSIMDBlock _store = {};
  friend class FBFixedAudioBlock;

public:
  void CopyFrom(FBFixedCVBlock const& rhs);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedCVBlock);
  
  static int Count() { return FB_FIXED_BLOCK_SIZE; }

  void SetToZero() { _store.SetToZero(); }
  float& operator[](int sample) { return _store[sample]; }
  float const& operator[](int sample) const { return _store[sample]; }
};

inline void 
FBFixedCVBlock::CopyFrom(FBFixedCVBlock const& rhs)
{
  for (int i = 0; i < Count(); i++)
    (*this)[i] = rhs[i];
}