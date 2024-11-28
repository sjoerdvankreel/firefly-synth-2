#pragma once

#include <playground_base/shared/FBPlugConfig.hpp>
#include <playground_base/shared/FBObjectLifetime.hpp>
#include <array>

class FBPipelineAudioBlock;

class alignas(FB_PLUG_BLOCK_ALIGN)
FBPlugSignalBlock
{
  std::array<float, FB_PLUG_BLOCK_SIZE> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugSignalBlock);
  
  float& operator[](int sample)
  { return _store[sample]; }
  float const& operator[](int sample) const
  { return _store[sample]; }

  int Count() const 
  { return FB_PLUG_BLOCK_SIZE; }
  void Fill(float val) 
  { std::fill(_store.begin(), _store.end(), val); }
};

class alignas(FB_PLUG_BLOCK_ALIGN) FBPlugAudioBlock
{
  std::array<FBPlugSignalBlock, 2> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugAudioBlock);
  void CopyFrom(FBPipelineAudioBlock const& pipeline);

  FBPlugSignalBlock& operator[](int channel)
  { return _store[channel]; }
  FBPlugSignalBlock const& operator[](int channel) const
  { return _store[channel]; }

  int Count() const 
  { return FB_PLUG_BLOCK_SIZE; }
  void Fill(float val) 
  { for (int ch = 0; ch < 2; ch++) _store[ch].Fill(val); }
};