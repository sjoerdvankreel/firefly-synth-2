#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedSIMDBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

#include <array>

class alignas(FBSIMDVectorByteCount) FBFixedAudioBlock
{
  std::array<FBFixedSIMDBlock, 2> _store = {};

public:

  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedAudioBlock);
  static int Count() { return FBFixedBlockSize; }
  FBFixedSIMDBlock& operator[](int ch) { return _store[ch]; }
  FBFixedSIMDBlock const& operator[](int ch) const { return _store[ch]; }

  // TODO simd
  void SetToZero();
  void CopyFrom(FBFixedAudioBlock const& rhs);
  void CopyFrom(FBBufferAudioBlock const& rhs);

  void InPlaceAdd(FBFixedAudioBlock const& rhs);
  void InPlaceMultiplyBy(FBFixedCVBlock const& rhs);
  void InPlaceMultiplyByOneMinus(FBFixedCVBlock const& rhs);
  void InPlaceFMA(FBFixedAudioBlock const& b, FBFixedCVBlock const& c);
};

inline void
FBFixedAudioBlock::SetToZero()
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      _store[ch][s] = 0.0f;
}

inline void
FBFixedAudioBlock::CopyFrom(FBFixedAudioBlock const& rhs)
{
  assert(rhs.Count() >= Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      _store[ch][s] = rhs[ch][s];
}

inline void 
FBFixedAudioBlock::CopyFrom(FBBufferAudioBlock const& rhs)
{
  assert(rhs.Count() >= Count());
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      _store[ch][s] = rhs[ch][s];
}

inline void
FBFixedAudioBlock::InPlaceAdd(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      _store[ch][s] += rhs[ch][s];
}

inline void
FBFixedAudioBlock::InPlaceMultiplyBy(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      _store[ch][s] *= rhs[s];
}

inline void
FBFixedAudioBlock::InPlaceMultiplyByOneMinus(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      _store[ch][s] *= 1.0f - rhs[s];
}

inline void 
FBFixedAudioBlock::InPlaceFMA(FBFixedAudioBlock const& b, FBFixedCVBlock const& c)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < Count(); s++)
      _store[ch][s] += b[ch][s] * c[s];
}