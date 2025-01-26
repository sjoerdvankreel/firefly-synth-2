#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>

class alignas(sizeof(FBFloatVector)) FBFixedFloatAudioBlock final
{
  std::array<FBFixedFloatBlock, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedFloatAudioBlock);

  void Clear();
  void Mul(FBFixedFloatBlock const& rhs);
  void Add(FBFixedFloatAudioBlock const& rhs);
  void CopyFrom(FBFixedFloatAudioBlock const& rhs);
  template <class Op> void Transform(Op op);

  void StoreToFloatArray(FBFixedFloatAudioArray& array) const;
  void LoadFromFloatArray(FBFixedFloatAudioArray const& array);
  void StoreCastToDoubleArray(FBFixedDoubleAudioArray& array) const;
  void LoadCastFromDoubleArray(FBFixedDoubleAudioArray const& array);

  FBFixedFloatBlock& operator[](int ch) { return _store[ch]; }
  FBFixedFloatBlock const& operator[](int ch) const { return _store[ch]; }
};

template <class Op>
inline void
FBFixedFloatAudioBlock::Transform(Op op)
{
  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedFloatVectors; v++)
      (*this)[ch][v] = op(ch, v);
}

inline void
FBFixedFloatAudioBlock::Clear()
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].Clear();
}

inline void
FBFixedFloatAudioBlock::Mul(FBFixedFloatBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].Mul(rhs);
}

inline void
FBFixedFloatAudioBlock::Add(FBFixedFloatAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].Add(rhs[ch]);
}

inline void
FBFixedFloatAudioBlock::CopyFrom(FBFixedFloatAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].CopyFrom(rhs[ch]);
}

inline void 
FBFixedFloatAudioBlock::StoreToFloatArray(FBFixedFloatAudioArray& array) const
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].StoreToFloatArray(array.data[ch]);
}

inline void 
FBFixedFloatAudioBlock::LoadFromFloatArray(FBFixedFloatAudioArray const& array)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].LoadFromFloatArray(array.data[ch]);
}

inline void 
FBFixedFloatAudioBlock::StoreCastToDoubleArray(FBFixedDoubleAudioArray& array) const
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].StoreCastToDoubleArray(array.data[ch]);
}

inline void 
FBFixedFloatAudioBlock::LoadCastFromDoubleArray(FBFixedDoubleAudioArray const& array)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].LoadCastFromDoubleArray(array.data[ch]);
}