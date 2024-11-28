#pragma once

#include <playground_base/shared/FBObjectLifetime.hpp>
#include <playground_base/pipeline/FBPipelineAudioBlock.hpp>

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

class FBHostAudioBlock
{
  int _count;
  std::array<float*, FB_CHANNELS_STEREO> _store;
  friend class FBPipelineAudioBlock;

public:
  FB_COPY_MOVE_DEFCTOR(FBHostAudioBlock);
  FBHostAudioBlock(float* l, float* r, int count) :
  _count(count), _store({ l, r }) {}

  float const* operator[](int channel) const { return _store[channel]; }

  void Fill(int from, int to, float val)
  {
    assert(0 <= from && from <= to && to <= _count);
    for (int ch = 0; ch < 2; ch++)
      for (int s = from; s < to; s++)
        _store[ch][s] = val;
  }

  void CopyFrom(FBPipelineAudioBlock const& pipeline, int srcOffset, int count);
  void Set(int s, float l, float r)
  {
    assert(0 <= s && s < _count);
    _store[0][s] = l;
    _store[1][s] = r;
  }

  float& At(int ch, int s)
  {
    assert(0 <= ch && ch < 2);
    assert(0 <= s && s < _count);
    return _store[ch][s];
  }
  
  float const& At(int ch, int s) const
  {
    assert(0 <= ch && ch < 2);
    assert(0 <= s && s < _count);
    return _store[ch][s];
  }

  int Count() const { return _count; }
};

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