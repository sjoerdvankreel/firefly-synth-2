#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedVectorBlock.hpp>

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
  void Add(FBFixedAudioBlock const& rhs);
  void CopyFrom(FBFixedAudioBlock const& rhs);
  void CopyFrom(FBBufferAudioBlock const& rhs);

  FBFixedVectorBlock& operator[](int ch) { return _store[ch]; }
  FBFixedVectorBlock const& operator[](int ch) const { return _store[ch]; }

  float Sample(int ch, int index) const { return _store[ch].Sample(index); }
  void Sample(int ch, int index, float val) { _store[ch].Sample(index, val); }
  void Samples(int index, float val) { _store[0].Sample(index, val); _store[1].Sample(index, val); }
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
FBFixedAudioBlock::CopyFrom(FBFixedAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedBlockVectors; v++)
      (*this)[ch][v] = rhs[ch][v];
}

inline void
FBFixedAudioBlock::CopyFrom(FBBufferAudioBlock const& rhs)
{
  for (int ch = 0; ch < 2; ch++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      Sample(ch, s, rhs[ch][s]);
}