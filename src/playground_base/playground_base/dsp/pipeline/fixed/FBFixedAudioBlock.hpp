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

  void SetLRTo(int sample, float val);
  void CopyFrom(FBBufferAudioBlock const& rhs);
  static int Count() { return FBFixedBlockSize; }

  FBFixedSIMDBlock& operator[](int ch) { return _store[ch]; }
  FBFixedSIMDBlock const& operator[](int ch) const { return _store[ch]; }

  FBFixedAudioBlock& FB_SIMD_CALL operator+=(FBFixedCVBlock const& rhs);
  FBFixedAudioBlock& FB_SIMD_CALL operator-=(FBFixedCVBlock const& rhs);
  FBFixedAudioBlock& FB_SIMD_CALL operator*=(FBFixedCVBlock const& rhs);
  FBFixedAudioBlock& FB_SIMD_CALL operator/=(FBFixedCVBlock const& rhs);

  FBFixedAudioBlock& FB_SIMD_CALL operator+=(FBFixedAudioBlock const& rhs);
  FBFixedAudioBlock& FB_SIMD_CALL operator-=(FBFixedAudioBlock const& rhs);
  FBFixedAudioBlock& FB_SIMD_CALL operator*=(FBFixedAudioBlock const& rhs);
  FBFixedAudioBlock& FB_SIMD_CALL operator/=(FBFixedAudioBlock const& rhs);

  void FB_SIMD_CALL SetToZero();
  void FB_SIMD_CALL SetToSineOfTwoPi();
  void FB_SIMD_CALL SetToUnipolarSineOfTwoPi();
  void FB_SIMD_CALL CopyFrom(FBFixedAudioBlock const& rhs);
  void FB_SIMD_CALL MultiplyByOneMinus(FBFixedCVBlock const& rhs);
  void FB_SIMD_CALL FMA1(FBFixedAudioBlock const& b, FBFixedCVBlock const& c);
  void FB_SIMD_CALL FMA2(FBFixedAudioBlock const& b, FBFixedCVBlock const& c, FBFixedCVBlock const& d);
};

inline void 
FBFixedAudioBlock::SetLRTo(int sample, float val)
{
  for(int ch = 0; ch < 2; ch++)
    _store[ch][sample] = val;
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
FBFixedAudioBlock::SetToZero()
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].SetToZero();
}

inline void
FBFixedAudioBlock::SetToSineOfTwoPi()
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].SetToSineOfTwoPi();
}

inline void
FBFixedAudioBlock::SetToUnipolarSineOfTwoPi()
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].SetToUnipolarSineOfTwoPi();
}

inline void
FBFixedAudioBlock::CopyFrom(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].CopyFrom(rhs._store[ch]);
}

inline void 
FBFixedAudioBlock::MultiplyByOneMinus(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].MultiplyByOneMinus(rhs._store);
}

inline void
FBFixedAudioBlock::FMA1(FBFixedAudioBlock const& b, FBFixedCVBlock const& c)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].FMA1(b._store[ch], c._store);
}

inline void
FBFixedAudioBlock::FMA2(FBFixedAudioBlock const& b, FBFixedCVBlock const& c, FBFixedCVBlock const& d)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].FMA2(b._store[ch], c._store, d._store);
}

inline FBFixedAudioBlock&
FBFixedAudioBlock::operator+=(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] += rhs._store;
  return *this;
}

inline FBFixedAudioBlock&
FBFixedAudioBlock::operator-=(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] -= rhs._store;
  return *this;
}

inline FBFixedAudioBlock&
FBFixedAudioBlock::operator*=(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] *= rhs._store;
  return *this;
}

inline FBFixedAudioBlock&
FBFixedAudioBlock::operator/=(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] /= rhs._store;
  return *this;
}

inline FBFixedAudioBlock& 
FBFixedAudioBlock::operator+=(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] += rhs._store[ch];
  return *this;
}

inline FBFixedAudioBlock& 
FBFixedAudioBlock::operator-=(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] -= rhs._store[ch];
  return *this;
}

inline FBFixedAudioBlock&
FBFixedAudioBlock::operator*=(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] *= rhs._store[ch];
  return *this;
}

inline FBFixedAudioBlock&
FBFixedAudioBlock::operator/=(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] /= rhs._store[ch];
  return *this;
}
