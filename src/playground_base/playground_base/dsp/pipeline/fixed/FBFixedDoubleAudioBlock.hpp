#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedDoubleBlock.hpp>

class alignas(sizeof(FBDoubleVector)) FBFixedDoubleAudioBlock
{
  std::array<FBFixedDoubleBlock, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedDoubleAudioBlock);

  void Clear();
  void Add(FBFixedDoubleAudioBlock const& rhs);
  void CopyFrom(FBFixedDoubleAudioBlock const& rhs);
  template <class Op> void Transform(Op op);

  void StoreToFloatArray(FBFixedFloatAudioArray& array) const;
  void LoadFromFloatArray(FBFixedFloatAudioArray const& array);
  void StoreToDoubleArray(FBFixedDoubleAudioArray& array) const;
  void LoadFromDoubleArray(FBFixedFloatAudioArray const& array);

  FBFixedDoubleBlock& operator[](int ch) { return _store[ch]; }
  FBFixedDoubleBlock const& operator[](int ch) const { return _store[ch]; }
};

template <class Op>
inline void
FBFixedDoubleAudioBlock::Transform(Op op)
{
  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedFloatVectors; v++)
      (*this)[ch][v] = op(ch, v);
}

inline void
FBFixedDoubleAudioBlock::Clear()
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].Clear();
}

inline void
FBFixedDoubleAudioBlock::Add(FBFixedDoubleAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].Add(rhs[ch]);
}

inline void
FBFixedDoubleAudioBlock::CopyFrom(FBFixedDoubleAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].CopyFrom(rhs[ch]);
}