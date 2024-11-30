#pragma once

#include <playground_base/dsp/shared/FBAnyAudioBlock.hpp>
#include <playground_base/base/shared/FBObjectLifetime.hpp>

class FBHostAudioBlock:
public FBAnyAudioBlock<FBHostAudioBlock, float*>
{
  int _count = 0;

public:
  int Count() const { return _count; }  
  FB_COPY_MOVE_DEFCTOR(FBHostAudioBlock);

  FBHostAudioBlock(float* l, float* r, int count):
  FBAnyAudioBlock(std::move(l), std::move(r)), _count(count) {}
};