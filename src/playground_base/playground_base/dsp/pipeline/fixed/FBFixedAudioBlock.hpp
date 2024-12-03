#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/shared/FBAnyAudioBlock.hpp>

#include <array>

class alignas(FB_FIXED_BLOCK_ALIGN) FBFixedAudioBlock:
public FBAnyAudioBlock<
  FBFixedAudioBlock,
  std::array<float, FB_FIXED_BLOCK_SIZE>>
{
public:
  StoreT& operator[](int channel)
  { return _store[channel]; }

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedAudioBlock);
  FBFixedAudioBlock(): FBAnyAudioBlock({}, {}) {}
  static int Count() { return FB_FIXED_BLOCK_SIZE; }
};