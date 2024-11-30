#pragma once

#include <playground_base/dsp/shared/FBAnyAudioBlock.hpp>
#include <playground_base/base/plug/FBPlugConfig.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

class alignas(FB_PLUG_BLOCK_ALIGN) FBPlugAudioBlock:
public FBAnyAudioBlock<
  FBPlugAudioBlock, 
  std::array<float, FB_PLUG_BLOCK_SIZE>>
{
public:
  StoreT& operator[](int channel)
  { return _store[channel]; }

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugAudioBlock);
  FBPlugAudioBlock(): FBAnyAudioBlock({}, {}) {}
  static int Count() { return FB_PLUG_BLOCK_SIZE; }
};