#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedVectorBlock.hpp>
#include <playground_base/dsp/pipeline/buffer/FBBufferAudioBlock.hpp>

#include <array>

class FBBufferAudioBlock;

class alignas(sizeof(FBFloatVector)) FBFixedAudioBlock
{
  std::array<FBFixedVectorBlock, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedAudioBlock);

  template <class Op>
  void Transform(Op op);

  void Clear();
  void LoadUnaligned(float const* vals[2]);
  void StoreUnaligned(float* vals[2]) const;
  void Add(FBFixedAudioBlock const& rhs);
  void CopyFrom(FBFixedAudioBlock const& rhs);
  void CopyFrom(FBBufferAudioBlock const& rhs);

  FBFixedVectorBlock& operator[](int ch) { return _store[ch]; }
  FBFixedVectorBlock const& operator[](int ch) const { return _store[ch]; }
};

template <class Op>
void 
FBFixedAudioBlock::Transform(Op op)
{
  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedBlockVectors; v++)
      (*this)[ch][v] = op(ch, v);
}

inline void 
FBFixedAudioBlock::Clear() 
{ 
  for (int ch = 0; ch < 2; ch++) 
    _store[ch].Clear();
}

inline void
FBFixedAudioBlock::Add(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].Add(rhs._store[ch]);
}

inline void
FBFixedAudioBlock::LoadUnaligned(float const* vals[2])
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].LoadUnaligned(vals[ch]);
}

inline void
FBFixedAudioBlock::StoreUnaligned(float* vals[2]) const
{
  for (int ch = 0; ch < 2; ch++)
    _store[ch].StoreUnaligned(vals[ch]);
}

inline void
FBFixedAudioBlock::CopyFrom(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedBlockVectors; v++)
      (*this)[ch][v] = rhs[ch][v];
}

inline void
FBFixedAudioBlock::CopyFrom(FBBufferAudioBlock const& rhs)
{
  float const* lr[2] = { rhs[0].data(), rhs[1].data() };
  LoadUnaligned(lr);
}