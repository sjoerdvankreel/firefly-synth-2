#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

inline int constexpr FBFixedFloatVectors = FBFixedBlockSamples / FBVectorFloatCount;

class alignas(sizeof(FBFloatVector)) FBFixedFloatBlock
{
  std::array<FBFloatVector, FBFixedFloatVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedFloatBlock);

  template <class Op>
  void Transform(Op op);
  void Clear();
  void Add(FBFixedFloatBlock const& rhs);

  void LoadUnaligned(float const* vals);
  void StoreUnaligned(float* vals) const;
  FBFloatVector& operator[](int index) { return _store[index]; }
  FBFloatVector const& operator[](int index) const { return _store[index]; } 
};

template <class Op>
void
FBFixedFloatBlock::Transform(Op op)
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    (*this)[v] = op(v);
}

inline void
FBFixedFloatBlock::Clear()
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v] = 0.0f;
}

inline void
FBFixedFloatBlock::Add(FBFixedFloatBlock const& rhs)
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v] += rhs._store[v];
}

inline void
FBFixedFloatBlock::StoreUnaligned(float* vals) const
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v].store_unaligned(vals + v * FBVectorFloatCount);
}

inline void
FBFixedFloatBlock::LoadUnaligned(float const* vals)
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v] = FBFloatVector::load_unaligned(vals + v * FBVectorFloatCount);
}