#pragma once

#include <playground_base/shared/FBPlugConfig.hpp>
#include <playground_base/shared/FBObjectLifetime.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>

#include <cassert>
#include <array>
#include <vector>

class FBPipelineAudioBlock;

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
  std::array<FBPlugSignalBlock, 2> _store;

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
  { for (int ch = 0; ch < 2; ch++) _store[ch].Fill(val); }
};