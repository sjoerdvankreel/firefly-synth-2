#pragma once

#include <playground_base/shared/FBObjectLifetime.hpp>
#include <array>
#include <vector>

class FBPipelineAudioBlock;

class FBHostAudioBlock
{
  int _count;
  std::array<float*, 2> _store;

public:
  FB_COPY_MOVE_NODEFCTOR(FBHostAudioBlock);
  FBHostAudioBlock(float* l, float* r, int count);

  int Count() const 
  { return _count; }
  float const* operator[](int channel) const 
  { return _store[channel]; }

  void Fill(int from, int to, float val);
  void CopyFrom(FBPipelineAudioBlock const& pipeline, int srcOffset, int count);
};