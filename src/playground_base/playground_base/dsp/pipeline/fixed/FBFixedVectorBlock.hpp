#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

inline int constexpr FBFixedBlockVectors = FBFixedBlockSamples / FBVectorFloatCount;

class alignas(sizeof(FBFloatVector)) FBFixedVectorBlock
{
  std::array<FBFloatVector, FBFixedBlockVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedVectorBlock);

  template <class Op>
  void Transform(Op op);

  void Clear();
  void LoadUnaligned(float const* vals);
  void StoreUnaligned(float* vals) const;
  void Add(FBFixedVectorBlock const& rhs);

  FBFloatVector& operator[](int index) { return _store[index]; }
  FBFloatVector const& operator[](int index) const { return _store[index]; } 
};

template <class Op>
void
FBFixedVectorBlock::Transform(Op op)
{
  for (int v = 0; v < FBFixedBlockVectors; v++)
    (*this)[v] = op(v);
}

inline void
FBFixedVectorBlock::Clear()
{
  for (int v = 0; v < FBFixedBlockVectors; v++)
    _store[v] = 0.0f;
}

void
FBFixedVectorBlock::Add(FBFixedVectorBlock const& rhs)
{
  for (int v = 0; v < FBFixedBlockVectors; v++)
    _store[v] += rhs._store[v];
}

inline void
FBFixedVectorBlock::StoreUnaligned(float* vals) const
{
  for (int v = 0; v < FBFixedBlockVectors; v++)
    _store[v].store_unaligned(vals + v * FBVectorFloatCount);
}

inline void
FBFixedVectorBlock::LoadUnaligned(float const* vals)
{
  for (int v = 0; v < FBFixedBlockVectors; v++)
    _store[v] = FBFloatVector::load_unaligned(vals + v * FBVectorFloatCount);
}