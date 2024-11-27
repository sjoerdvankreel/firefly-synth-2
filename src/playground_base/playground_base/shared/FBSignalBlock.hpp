#pragma once

#include <playground_base/shared/FBSharedUtility.hpp>

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
  friend class FBDynamicAudioBlock;

public:
  FB_COPY_MOVE_DEFCTOR(FBHostAudioBlock);
  FBHostAudioBlock(float* l, float* r, int count) :
  _count(count), _store({ l, r }) {}

  void SetToZero(int from, int to);
  int Count() const { return _count; }
};

class alignas(FB_PLUG_BLOCK_ALIGN) FBFixedCVBlock
{
  std::array<float, FB_PLUG_BLOCK_SIZE> _store;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedCVBlock);

  float& operator[](int index)
  { return _store[index]; }
  float const& operator[](int index) const
  { return _store[index]; }
  void Fill(float val) 
  { std::fill(_store.begin(), _store.end(), val); }
};

class alignas(FB_PLUG_BLOCK_ALIGN) FBFixedAudioBlock
{
  std::array<FBFixedCVBlock, FB_CHANNELS_STEREO> _store;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedAudioBlock);

  FBFixedCVBlock& operator[](int channel)
  { return _store[channel]; }
  FBFixedCVBlock const& operator[](int channel) const
  { return _store[channel]; }
  void Fill(float val) 
  { for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++) _store[ch].Fill(val); }
};

class FBDynamicAudioBlock
{
  std::array<std::vector<float>, FB_CHANNELS_STEREO> _store;

public:
  FBDynamicAudioBlock(int capacity);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBDynamicAudioBlock);
  
  int Count() const 
  { return static_cast<int>(_store[FB_CHANNEL_L].size()); }

  void AppendFrom(FBFixedAudioBlock const& fixed);
  void AppendFrom(FBHostAudioBlock const& raw);
  void MoveOneFixedBlockTo(FBFixedAudioBlock& fixed);
  void MoveOneRawBlockToAndPad(FBHostAudioBlock& raw);
};