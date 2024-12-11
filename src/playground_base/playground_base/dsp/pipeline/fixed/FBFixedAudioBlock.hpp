#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedVectorBlock.hpp>

#include <array>

class FBBufferAudioBlock;

class alignas(FBVectorByteCount) FBFixedAudioBlock
{
  std::array<FBFixedVectorBlock, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedAudioBlock);
  void CopyFrom(FBBufferAudioBlock const& rhs);

  float Sample(int ch, int index) const { return _store[ch].Sample(index); }
  void Sample(int ch, int index, float val) { _store[ch].Sample(index, val); }
  FBFloatVector& Vector(int ch, int index) { return _store[ch].Vector(index); }
  FBFloatVector const& Vector(int ch, int index) const { return _store[ch].Vector(index); }
};