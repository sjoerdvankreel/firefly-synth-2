#pragma once

#include <playground_base/base/shared/FBObjectLifetime.hpp>
#include <playground_base/dsp/shared/FBAnyAudioBlock.hpp>

#include <array>
#include <vector>

class FBPipelineAudioBlock;

class FBHostAudioBlock:
public FBAnyAudioBlock<FBHostAudioBlock>
{
  int _count;
  std::array<float*, 2> _store;

public:
  int Count() const 
  { return _count; }
  float const* operator[](int channel) const 
  { return _store[channel]; }

  void Fill(int from, int to, float val);
  FB_COPY_MOVE_NODEFCTOR(FBHostAudioBlock);
  FBHostAudioBlock(float* l, float* r, int count);
};