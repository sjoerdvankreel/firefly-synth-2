#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

inline int constexpr FBFixedBlockVectors = FBFixedBlockSamples / FBVectorFloatCount;

class alignas(FBVectorByteCount) FBFixedVectorBlock
{
  std::array<FBFloatVector, FBFixedBlockVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedVectorBlock);

  FBFloatVector& operator[](int index) { return _store[index]; }
  FBFloatVector const& operator[](int index) const { return _store[index]; }
  float Sample(int index) const { return (*this)[index / FBVectorFloatCount][index % FBVectorFloatCount]; }
  void Sample(int index, float val) { (*this)[index / FBVectorFloatCount][index % FBVectorFloatCount] = val; }

  void Clear() { for (int v = 0; v < FBFixedBlockVectors; v++) _store[v].Clear(); }
  void Add(FBFixedVectorBlock const& rhs) { for (int v = 0; v < FBFixedBlockVectors; v++) _store[v] += rhs._store[v]; }
};