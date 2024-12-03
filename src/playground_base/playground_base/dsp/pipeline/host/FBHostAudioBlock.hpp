#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBAnyAudioBlock.hpp>

class FBHostAudioBlock final:
public FBAnyAudioBlock<FBHostAudioBlock, float*>
{
  int _count = 0;

public:
  int Count() const { return _count; }  
  FB_COPY_MOVE_NODEFCTOR(FBHostAudioBlock);

  FBHostAudioBlock(float* l, float* r, int count):
  FBAnyAudioBlock(std::move(l), std::move(r)), _count(count) {}
  FBHostAudioBlock() : FBAnyAudioBlock(nullptr, nullptr) {}
};