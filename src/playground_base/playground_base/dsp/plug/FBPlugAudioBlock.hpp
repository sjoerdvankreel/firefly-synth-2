#pragma once

#include <playground_base/dsp/shared/FBAnyAudioBlock.hpp>
#include <playground_base/base/plug/FBPlugConfig.hpp>
#include <playground_base/base/shared/FBObjectLifetime.hpp>

#include <array>

class alignas(FB_PLUG_BLOCK_ALIGN) FBPlugAudioBlock:
public FBAnyAudioBlock<
  FBPlugAudioBlock, 
  std::array<float, FB_PLUG_BLOCK_SIZE>>
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugAudioBlock);
  int Count() const { return FB_PLUG_BLOCK_SIZE; }
};