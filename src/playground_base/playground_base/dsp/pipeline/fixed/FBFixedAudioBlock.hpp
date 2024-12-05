#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBAnyAudioBlock.hpp>

#include <array>

class alignas(FB_FIXED_BLOCK_ALIGN) FBFixedAudioBlock final:
public FBAnyAudioBlock<FBFixedAudioBlock, std::array<float, FB_FIXED_BLOCK_SIZE>>
{
public:
  StoreT& operator[](int channel)
  { return _store[channel]; }
  StoreT const& operator[](int channel) const
  { return _store[channel]; }

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedAudioBlock);
  FBFixedAudioBlock(): FBAnyAudioBlock({}, {}) {}
  void InPlaceAdd(FBFixedAudioBlock const& rhs);
  static int Count() { return FB_FIXED_BLOCK_SIZE; }
};

inline void 
FBFixedAudioBlock::InPlaceAdd(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      (*this)[ch][s] += rhs[ch][s];
}