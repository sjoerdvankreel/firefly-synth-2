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

  void StoreToFloat(FBFixedFloatBlock& block) const;
  void LoadFromFloat(std::array<float, FBFixedBlockSamples> const& block);

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

inline void
FBFixedDoubleBlock::StoreToFloat(FBFixedFloatBlock& block) const
{
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> floats;
  alignas(sizeof(FBDoubleVector)) std::array<double, FBVectorFloatCount> doubles;
  for (int v = 0; v < FBFixedFloatVectors; v++)
  {
    (*this)[v * 2 + 0].store_aligned(doubles.data());
    (*this)[v * 2 + 1].store_aligned(doubles.data() + 2);
    for (int i = 0; i < FBVectorFloatCount; i++)
      floats[i] = (float)doubles[i];
    block.LoadAligned(v, floats.data());
  }
}

inline void
FBFixedDoubleBlock::LoadFromFloat(std::array<float, FBFixedBlockSamples> const& block)
{
  alignas(sizeof(FBDoubleVector)) std::array<double, FBVectorFloatCount> doubles;
  for (int v = 0; v < FBFixedFloatVectors; v++)
  {
    for (int i = 0; i < FBVectorFloatCount; i++)
      doubles[i] = block[v * FBVectorFloatCount + i];
    (*this)[v * 2 + 0] = FBDoubleVector::load_aligned(doubles.data());
    (*this)[v * 2 + 1] = FBDoubleVector::load_aligned(doubles.data() + 2);
  }
}