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

  FBFloatVector& Vector(int index) { return _store[index]; }
  FBFloatVector const& Vector(int index) const { return _store[index]; }
  float Sample(int index) const { return Vector(index / FBVectorFloatCount)[index % FBVectorFloatCount]; }
  void Sample(int index, float val) { Vector(index / FBVectorFloatCount)[index % FBVectorFloatCount] = val; }
};