#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedSIMDBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBAnyAudioBlock.hpp>

class alignas(FBSIMDVectorAlign) FBFixedAudioBlock final:
public FBAnyAudioBlock<FBFixedAudioBlock, FBFixedSIMDBlock>
{
public:
  StoreT& operator[](int channel)
  { return _store[channel]; }
  StoreT const& operator[](int channel) const
  { return _store[channel]; }

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedAudioBlock);
  FBFixedAudioBlock(): FBAnyAudioBlock({}, {}) {}
  static int Count() { return FB_FIXED_BLOCK_SIZE; }

  void operator+=(FBFixedCVBlock const& rhs);
  void operator-=(FBFixedCVBlock const& rhs);
  void operator*=(FBFixedCVBlock const& rhs);
  void operator/=(FBFixedCVBlock const& rhs);
  void operator+=(FBFixedAudioBlock const& rhs);
  void operator-=(FBFixedAudioBlock const& rhs);
  void operator*=(FBFixedAudioBlock const& rhs);
  void operator/=(FBFixedAudioBlock const& rhs);
};

inline void
FBFixedAudioBlock::operator+=(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] += rhs._store;
}

inline void
FBFixedAudioBlock::operator-=(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] -= rhs._store;
}

inline void
FBFixedAudioBlock::operator*=(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] *= rhs._store;
}

inline void
FBFixedAudioBlock::operator/=(FBFixedCVBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] /= rhs._store;
}

inline void
FBFixedAudioBlock::operator+=(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] += rhs._store[ch];
}

inline void
FBFixedAudioBlock::operator-=(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] -= rhs._store[ch];
}

inline void
FBFixedAudioBlock::operator*=(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] *= rhs._store[ch];
}

inline void
FBFixedAudioBlock::operator/=(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch] /= rhs._store[ch];
}