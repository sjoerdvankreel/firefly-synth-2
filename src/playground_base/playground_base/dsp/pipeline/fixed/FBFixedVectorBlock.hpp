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

  FBFloatVector& operator[](int index) { return _store[index]; }
  FBFloatVector const& operator[](int index) const { return _store[index]; }
  float Sample(int index) const { return 0; }//TODO { return (*this)[index / FBVectorFloatCount].get(index % FBVectorFloatCount); } // TODO?
  void Sample(int index, float val) {} // TODO (*this)[index / FBVectorFloatCount][index % FBVectorFloatCount] = val; }

  void Clear() { for (int v = 0; v < FBFixedBlockVectors; v++) _store[v] = 0.0f; }
  void Add(FBFixedVectorBlock const& rhs) { for (int v = 0; v < FBFixedBlockVectors; v++) _store[v] += rhs._store[v]; }
};

template <class Op>
void
FBFixedVectorBlock::Transform(Op op)
{
  for (int v = 0; v < FBFixedBlockVectors; v++)
    (*this)[v] = op(v);
}