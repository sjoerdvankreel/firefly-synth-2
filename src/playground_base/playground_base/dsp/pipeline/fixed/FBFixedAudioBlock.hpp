#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedSIMDBlock.hpp>

class alignas(FBSIMDVectorAlign) FBFixedAudioBlock final
{
  std::array<FBFixedSIMDBlock, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedAudioBlock);
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