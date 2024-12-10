#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBSIMDVector.hpp>

#include <array>

inline int constexpr FBFixedSIMDBlockFloatVectorCount = 
FBFixedBlockSize / FBSIMDVectorFloatCount;

class alignas(alignof(FBSIMDFloatVector)) FBFixedSIMDBlock
{
  static inline int constexpr VectorCount = FBFixedSIMDBlockFloatVectorCount;
  std::array<FBSIMDFloatVector, VectorCount> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedSIMDBlock);

  float& operator[](int index);
  float operator[](int index) const;

  FBFixedSIMDBlock& FBSIMDCall operator+=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FBSIMDCall operator-=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FBSIMDCall operator*=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FBSIMDCall operator/=(FBFixedSIMDBlock const& rhs);
};

inline float& 
FBFixedSIMDBlock::operator[](int index)
{
  return _store[index / VectorCount][index % VectorCount];
}

inline float 
FBFixedSIMDBlock::operator[](int index) const
{
  return _store[index / VectorCount][index % VectorCount];
}

inline FBFixedSIMDBlock& FBSIMDCall 
FBFixedSIMDBlock::operator+=(FBFixedSIMDBlock const& rhs)
{
  for (int b = 0; b < VectorCount; b++)
    _store[b] += rhs._store[b];
  return *this;
}

inline FBFixedSIMDBlock& FBSIMDCall 
FBFixedSIMDBlock::operator-=(FBFixedSIMDBlock const& rhs)
{
  for (int b = 0; b < VectorCount; b++)
    _store[b] -= rhs._store[b];
  return *this;
}

inline FBFixedSIMDBlock& FBSIMDCall
FBFixedSIMDBlock::operator*=(FBFixedSIMDBlock const& rhs)
{
  for (int b = 0; b < VectorCount; b++)
    _store[b] *= rhs._store[b];
  return *this;
}

inline FBFixedSIMDBlock& FBSIMDCall
FBFixedSIMDBlock::operator/=(FBFixedSIMDBlock const& rhs)
{
  for (int b = 0; b < VectorCount; b++)
    _store[b] /= rhs._store[b];
  return *this;
}