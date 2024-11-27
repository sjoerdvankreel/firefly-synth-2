#pragma once

#include <playground_base/shared/FBObjectLifetime.hpp>

#include <cassert>
#include <array>
#include <vector>

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

  FBPlugSignalBlock& operator[](int channel)
  { return _store[channel]; }
  FBPlugSignalBlock const& operator[](int channel) const
  { return _store[channel]; }
  void Fill(float val) 
  { for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++) _store[ch].Fill(val); }
};

class FBPipelineAudioBlock
{
  std::array<std::vector<float>, FB_CHANNELS_STEREO> _store;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPipelineAudioBlock);
  
  int Count() const 
  { return static_cast<int>(_store[FB_CHANNEL_L].size()); }

  void Append(float l, float r) {
    _store[0].push_back(l);
    _store[1].push_back(r);
  }

  void Drop(int count)
  {
    assert(0 <= count && count <= _store[0].size());
    _store[0].erase(_store[0].begin(), _store[0].begin() + count);
    _store[1].erase(_store[1].begin(), _store[1].begin() + count);
  }

  float& At(int ch, int s)
  {
    assert(0 <= ch && ch < 2);
    assert(0 <= s && s < _store[0].size());
    return _store[ch][s];
  }

  float const& At(int ch, int s) const
  {
    assert(0 <= ch && ch < 2);
    assert(0 <= s && s < _store[0].size());
    return _store[ch][s];
  }

  void AppendFrom(FBHostAudioBlock const& raw);
  void MoveOneFixedBlockTo(FBPlugAudioBlock& fixed);
  void MoveOneRawBlockToAndPad(FBHostAudioBlock& raw);
};