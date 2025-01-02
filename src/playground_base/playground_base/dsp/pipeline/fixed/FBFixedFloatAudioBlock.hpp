#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>

class alignas(sizeof(FBFloatVector)) FBFixedFloatAudioBlock
{
  std::array<FBFixedFloatBlock, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedFloatAudioBlock);

  template <class Op>
  void Transform(Op op);

  void StoreToFloatArray(FBFixedFloatAudioArray& array) const;
  void LoadFromFloatArray(FBFixedFloatAudioArray const& array);
  void StoreToDoubleArray(FBFixedDoubleAudioArray& array) const;
  void LoadFromDoubleArray(FBFixedFloatAudioArray const& array);

  FBFixedFloatBlock& operator[](int ch) { return _store[ch]; }
  FBFixedFloatBlock const& operator[](int ch) const { return _store[ch]; }
};

template <class Op>
void
FBFixedFloatAudioBlock::Transform(Op op)
{
  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedFloatVectors; v++)
      (*this)[ch][v] = op(ch, v);
}