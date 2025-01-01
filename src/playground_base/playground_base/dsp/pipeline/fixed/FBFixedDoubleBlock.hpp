#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>

#include <array>

inline int constexpr FBFixedDoubleVectors = FBFixedBlockSamples / FBVectorDoubleCount;

class alignas(sizeof(FBDoubleVector)) FBFixedDoubleBlock
{
  std::array<FBDoubleVector, FBFixedDoubleVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedDoubleBlock);

  template <class Op>
  void Transform(Op op);
  FBDoubleVector& operator[](int index) { return _store[index]; }
  FBDoubleVector const& operator[](int index) const { return _store[index]; }
};

template <class Op>
void
FBFixedDoubleBlock::Transform(Op op)
{
  for (int v = 0; v < FBFixedDoubleVectors; v++)
    (*this)[v] = op(v);
}
