#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBSIMDVector.hpp>

#include <array>

class alignas(FBSIMDVectorAlign) FBFixedSIMDBlock
{
  static int constexpr VectorCount = FB_FIXED_BLOCK_SIZE / FBSIMDVectorFloatCount;
  std::array<FBSIMDFloatVector, VectorCount> _store = {};

public:
  FB_NOCOPY_MOVE_DEFCTOR(FBFixedSIMDBlock);

  void SetToZero();
  float& operator[](int i);
  float const operator[](int i) const;

  void operator+=(FBFixedSIMDBlock const& rhs);
  void operator-=(FBFixedSIMDBlock const& rhs);
  void operator*=(FBFixedSIMDBlock const& rhs);
  void operator/=(FBFixedSIMDBlock const& rhs);
};

inline void
FBFixedSIMDBlock::SetToZero()
{
  for (int i = 0; i < VectorCount; i++)
    _store[i].SetToZero();
}

inline void 
FBFixedSIMDBlock::operator+=(FBFixedSIMDBlock const& rhs)
{
  for (int i = 0; i < VectorCount; i++)
    _store[i] += rhs._store[i];
}

inline void 
FBFixedSIMDBlock::operator-=(FBFixedSIMDBlock const& rhs)
{
  for (int i = 0; i < VectorCount; i++)
    _store[i] -= rhs._store[i];
}

inline void 
FBFixedSIMDBlock::operator*=(FBFixedSIMDBlock const& rhs)
{
  for (int i = 0; i < VectorCount; i++)
    _store[i] *= rhs._store[i];
}

inline void 
FBFixedSIMDBlock::operator/=(FBFixedSIMDBlock const& rhs)
{
  for (int i = 0; i < VectorCount; i++)
    _store[i] /= rhs._store[i];
}