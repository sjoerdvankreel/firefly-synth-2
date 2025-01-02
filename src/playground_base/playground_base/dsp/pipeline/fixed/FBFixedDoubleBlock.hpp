#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>

class alignas(sizeof(FBDoubleVector)) FBFixedDoubleBlock
{
  std::array<FBDoubleVector, FBFixedDoubleVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedDoubleBlock);

  void Clear();
  template <class Op> void Transform(Op op);

  void StoreToFloatArray(FBFixedFloatArray& array) const;
  void LoadFromFloatArray(FBFixedFloatArray const& array);
  void StoreToDoubleArray(FBFixedDoubleArray& array) const;
  void LoadFromDoubleArray(FBFixedFloatArray const& array);

  FBDoubleVector& operator[](int index) { return _store[index]; }
  FBDoubleVector const& operator[](int index) const { return _store[index]; }
};

template <class Op>
inline void
FBFixedDoubleBlock::Transform(Op op)
{
  for (int v = 0; v < FBFixedDoubleVectors; v++)
    (*this)[v] = op(v);
}

inline void
FBFixedDoubleBlock::Clear()
{
  Transform([](auto) { return 0.0; });
}