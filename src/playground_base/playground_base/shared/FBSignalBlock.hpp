#pragma once

#include <playground_base/shared/FBObjectLifetime.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>

#include <cassert>
#include <array>
#include <vector>

class FBPipelineAudioBlock;

// TODO drop this stuff
#define FB_CHANNEL_L 0
#define FB_CHANNEL_R 1
#define FB_CHANNELS_STEREO 2

#define FB_PLUG_BLOCK_SIZE 2048 // TODO make it good
#define FB_PLUG_BLOCK_ALIGN (FB_PLUG_BLOCK_SIZE * sizeof(float))

class alignas(FB_PLUG_BLOCK_ALIGN) FBPlugSignalBlock
{
  std::array<float, FB_PLUG_BLOCK_SIZE> _store;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugSignalBlock);

  int Count() const { return FB_PLUG_BLOCK_SIZE; }
  float& operator[](int index)
  { return _store[index]; }
  float const& operator[](int index) const
  { return _store[index]; }
  void Fill(float val) 
  { std::fill(_store.begin(), _store.end(), val); }
};

class alignas(FB_PLUG_BLOCK_ALIGN) FBPlugAudioBlock
{
  std::array<FBPlugSignalBlock, FB_CHANNELS_STEREO> _store;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugAudioBlock);

  void CopyFrom(FBPipelineAudioBlock const& pipeline)
  {
    assert(pipeline.Count() >= Count());
    for (int ch = 0; ch < 2; ch++)
      for (int s = 0; s < Count(); s++)
        _store[ch][s] = pipeline[ch][s];
  }

  int Count() const { return FB_PLUG_BLOCK_SIZE; }
  FBPlugSignalBlock& operator[](int channel)
  { return _store[channel]; }
  FBPlugSignalBlock const& operator[](int channel) const
  { return _store[channel]; }
  void Fill(float val) 
  { for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++) _store[ch].Fill(val); }
};