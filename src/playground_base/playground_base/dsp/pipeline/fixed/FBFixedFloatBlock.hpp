#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>

class alignas(sizeof(FBFloatVector)) FBFixedFloatBlock
{
  std::array<FBFloatVector, FBFixedFloatVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedFloatBlock);

  void Clear();
  void Add(FBFixedFloatBlock const& rhs);
  void CopyFrom(FBFixedFloatBlock const& rhs);
  template <class Op> void Transform(Op op);

  void StoreToFloatArray(FBFixedFloatArray& array) const;
  void LoadFromFloatArray(FBFixedFloatArray const& array);
  void StoreCastToDoubleArray(FBFixedDoubleArray& array) const;
  void LoadCastFromDoubleArray(FBFixedDoubleArray const& array);

  FBFloatVector& operator[](int index) { return _store[index]; }
  FBFloatVector const& operator[](int index) const { return _store[index]; } 
};

template <class Op> 
inline void
FBFixedFloatBlock::Transform(Op op)
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    (*this)[v] = op(v);
}

inline void
FBFixedFloatBlock::Clear()
{
  Transform([](auto) { return 0.0f; });
}

inline void
FBFixedFloatBlock::Add(FBFixedFloatBlock const& rhs)
{
  Transform([&](int v) { return _store[v] + rhs[v]; });
}

inline void
FBFixedFloatBlock::CopyFrom(FBFixedFloatBlock const& rhs)
{
  Transform([&](int v) { return rhs[v]; });
}

inline void 
FBFixedFloatBlock::StoreToFloatArray(FBFixedFloatArray& array) const
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v].store_aligned(array.data.data() + v * FBVectorFloatCount);
}

inline void
FBFixedFloatBlock::LoadFromFloatArray(FBFixedFloatArray const& array)
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v] = FBFloatVector::load_aligned(array.data.data() + v * FBVectorFloatCount);
}

inline void
FBFixedFloatBlock::StoreCastToDoubleArray(FBFixedDoubleArray& array) const
{
  FBFixedFloatArray floats;
  StoreToFloatArray(floats);
  for (int s = 0; s < FBFixedBlockSamples; s++)
    array.data[s] = floats.data[s];
}

inline void
FBFixedFloatBlock::LoadCastFromDoubleArray(FBFixedDoubleArray const& array)
{
  FBFixedFloatArray floats;
  for (int s = 0; s < FBFixedBlockSamples; s++)
    floats.data[s] = (float)array.data[s];
  LoadFromFloatArray(floats);
}