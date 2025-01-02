#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>

class alignas(sizeof(FBFloatVector)) FBFixedFloatAudioBlock
{
  std::array<FBFixedFloatBlock, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedFloatAudioBlock);

  void Clear();
  void Add(FBFixedFloatAudioBlock const& rhs);
  void CopyFrom(FBFixedFloatAudioBlock const& rhs);
  template <class Op> void Transform(Op op);

  void StoreToFloatArray(FBFixedFloatAudioArray& array) const;
  void LoadFromFloatArray(FBFixedFloatAudioArray const& array);
  void StoreToDoubleArray(FBFixedDoubleAudioArray& array) const;
  void LoadFromDoubleArray(FBFixedFloatAudioArray const& array);

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