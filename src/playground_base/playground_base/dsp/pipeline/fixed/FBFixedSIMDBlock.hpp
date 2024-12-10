#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBSIMDVector.hpp>

#include <array>

inline int constexpr FBFixedSIMDBlockFloatVectorCount = 
FBFixedBlockSize / FBSIMDVectorFloatCount;

class alignas(alignof(FBSIMDFloatVector)) FBFixedSIMDBlock
{
  std::array<FBSIMDFloatVector, FBFixedSIMDBlockFloatVectorCount> _store = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedSIMDBlock);

  FBFixedSIMDBlock& FBSIMDCall operator+=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FBSIMDCall operator-=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FBSIMDCall operator*=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FBSIMDCall operator/=(FBFixedSIMDBlock const& rhs);
};

inline FBFixedSIMDBlock& FBSIMDCall 
FBFixedSIMDBlock::operator+=(FBFixedSIMDBlock const& rhs)
{
  for (int b = 0; b < FBFixedSIMDBlockFloatVectorCount; b++)
    _store[b] += rhs._store[b];
  return *this;
}

inline FBFixedSIMDBlock& FBSIMDCall 
FBFixedSIMDBlock::operator-=(FBFixedSIMDBlock const& rhs)
{
  for (int b = 0; b < FBFixedSIMDBlockFloatVectorCount; b++)
    _store[b] -= rhs._store[b];
  return *this;
}

inline FBFixedSIMDBlock& FBSIMDCall
FBFixedSIMDBlock::operator*=(FBFixedSIMDBlock const& rhs)
{
  for (int b = 0; b < FBFixedSIMDBlockFloatVectorCount; b++)
    _store[b] *= rhs._store[b];
  return *this;
}

inline FBFixedSIMDBlock& FBSIMDCall
FBFixedSIMDBlock::operator/=(FBFixedSIMDBlock const& rhs)
{
  for (int b = 0; b < FBFixedSIMDBlockFloatVectorCount; b++)
    _store[b] /= rhs._store[b];
  return *this;
}