#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBSIMDVector.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBAnyAudioBlock.hpp>

#include <array>

class alignas(FB_SIMD_VECTOR_ALIGN) FBFixedAudioBlock final:
public FBAnyAudioBlock<FBFixedAudioBlock, std::array<float, FB_FIXED_BLOCK_SIZE>>
{
public:
  StoreT& operator[](int channel)
  { return _store[channel]; }
  StoreT const& operator[](int channel) const
  { return _store[channel]; }

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedAudioBlock);
  FBFixedAudioBlock(): FBAnyAudioBlock({}, {}) {}
  static int Count() { return FB_FIXED_BLOCK_SIZE; }

  // TODO simd
  void InPlaceAdd(FBFixedAudioBlock const& rhs);
  void InPlaceMultiplyBy(FBFixedCVBlock const& rhs);
  void InPlaceMultiplyByOneMinus(FBFixedCVBlock const& rhs);
  void InPlaceFMA(FBFixedAudioBlock const& b, FBFixedCVBlock const& c);
};

inline void
FBFixedAudioBlock::InPlaceAdd(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      (*this)[ch][s] += rhs[ch][s];
}

inline void
FBFixedAudioBlock::InPlaceMultiplyBy(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      (*this)[ch][s] *= rhs[s];
}

inline void
FBFixedAudioBlock::InPlaceMultiplyByOneMinus(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      (*this)[ch][s] *= 1.0f - rhs[s];
}

inline void 
FBFixedAudioBlock::InPlaceFMA(FBFixedAudioBlock const& b, FBFixedCVBlock const& c)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      (*this)[ch][s] += b[ch][s] * c[s];
}