#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedSIMDBlock.hpp>

#include <array>
#include <cassert>

class alignas(FBSIMDVectorByteCount)
FBFixedCVBlock final
{
  FBFixedSIMDBlock _store = {};
  friend class FBFixedAudioBlock;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedCVBlock);

  static int Count() { return FBFixedBlockSize; }
  float& operator[](int sample) { return _store[sample]; }
  float const& operator[](int sample) const { return _store[sample]; }

  void FB_SIMD_CALL SetToZero() { _store.SetToZero(); }
  void FB_SIMD_CALL SetToSineOfTwoPi() { _store.SetToSineOfTwoPi(); }
  void FB_SIMD_CALL SetToUnipolarSineOfTwoPi() { _store.SetToUnipolarSineOfTwoPi(); }
  void FB_SIMD_CALL CopyFrom(FBFixedCVBlock const& rhs) { _store.CopyFrom(rhs._store); }
};