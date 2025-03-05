#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>

class alignas(sizeof(FBDoubleVector)) FBFixedDoubleBlock final
{
  std::array<FBDoubleVector, FBFixedDoubleVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedDoubleBlock);

  void Clear();
  void Add(FBFixedDoubleBlock const& rhs);
  void CopyFrom(FBFixedDoubleBlock const& rhs);
  template <class Op> void Transform(Op op);

  void StoreToDoubleArray(FBFixedDoubleArray& array) const;
  void LoadFromDoubleArray(FBFixedDoubleArray const& array);
  void StoreCastToFloatArray(FBFixedFloatArray& array) const;
  void LoadCastFromFloatArray(FBFixedFloatArray const& array);

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

inline void
FBFixedDoubleBlock::Add(FBFixedDoubleBlock const& rhs)
{
  Transform([&](int v) { return _store[v] + rhs[v]; });
}

inline void
FBFixedDoubleBlock::CopyFrom(FBFixedDoubleBlock const& rhs)
{
  Transform([&](int v) { return rhs[v]; });
}

inline void
FBFixedDoubleBlock::StoreToDoubleArray(FBFixedDoubleArray& array) const
{
  for (int v = 0; v < FBFixedDoubleVectors; v++)
    _store[v].store_aligned(array.data.data() + v * FBVectorDoubleCount);
}

inline void
FBFixedDoubleBlock::LoadFromDoubleArray(FBFixedDoubleArray const& array)
{
  for (int v = 0; v < FBFixedDoubleVectors; v++)
    _store[v] = FBDoubleVector::load_aligned(array.data.data() + v * FBVectorDoubleCount);
}

inline void
FBFixedDoubleBlock::StoreCastToFloatArray(FBFixedFloatArray& array) const
{
  FBFixedDoubleArray doubles;
  StoreToDoubleArray(doubles);
  for (int s = 0; s < FBFixedBlockSamples; s++)
    array.data[s] = static_cast<float>(doubles.data[s]);
}

inline void
FBFixedDoubleBlock::LoadCastFromFloatArray(FBFixedFloatArray const& array)
{
  FBFixedDoubleArray doubles;
  for (int s = 0; s < FBFixedBlockSamples; s++)
    doubles.data[s] = array.data[s];
  LoadFromDoubleArray(doubles);
}