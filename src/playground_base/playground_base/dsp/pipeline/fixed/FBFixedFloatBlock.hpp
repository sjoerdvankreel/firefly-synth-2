#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>

class alignas(sizeof(FBFloatVector)) FBFixedFloatBlock
{
  std::array<FBFloatVector, FBFixedFloatVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedFloatBlock);

  template <class Op>
  void Transform(Op op);

  void StoreToFloatArray(FBFixedFloatArray& array) const;
  void LoadFromFloatArray(FBFixedFloatArray const& array);
  void StoreToDoubleArray(FBFixedDoubleArray& array) const;
  void LoadFromDoubleArray(FBFixedFloatArray const& array);

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