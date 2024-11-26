#pragma once

#include <playground_base/shared/FBSharedUtility.hpp>

#include <array>
#include <vector>
#include <cassert>

#define FB_CHANNEL_L 0
#define FB_CHANNEL_R 1
#define FB_CHANNELS_STEREO 2
#define FB_FIXED_BLOCK_SIZE 1024 // TODO make it good
#define FB_FIXED_BLOCK_ALIGN (FB_FIXED_BLOCK_SIZE * sizeof(float))

class FBRawAudioBlockView
{
  int _count;
  std::array<float*, FB_CHANNELS_STEREO> _store;
  friend class FBDynamicAudioBlock;

public:
  FB_COPY_NOMOVE_DEFCTOR(FBRawAudioBlockView);
  FBRawAudioBlockView(float* l, float* r, int count) :
  _count(count), _store({ l, r }) {}

  void SetToZero(int from, int to);
  int Count() const { return _count; }
};

inline void
FBRawAudioBlockView::SetToZero(int from, int to)
{
  assert(0 <= from && from <= to && to < _count);
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
    for (int i = from; i < to; i++)
      _store[ch][i] = 0.0f;
}

class alignas(FB_FIXED_BLOCK_ALIGN) FBFixedCVBlock
{
  std::array<float, FB_FIXED_BLOCK_SIZE> _store;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedCVBlock);

  float& operator[](int index)
  { return _store[index]; }
  float const& operator[](int index) const
  { return _store[index]; }
  void Fill(float val) 
  { std::fill(_store.begin(), _store.end(), val); }
};

class alignas(FB_FIXED_BLOCK_ALIGN) FBFixedAudioBlock
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

  void AppendFrom(FBFixedAudioBlock const& fixed);
  void AppendFrom(FBRawAudioBlockView const& raw);
  void MoveOneFixedBlockTo(FBFixedAudioBlock& fixed);
  void MoveOneRawBlockToAndPad(FBRawAudioBlockView& raw);
  int Count() const { return static_cast<int>(_store[FB_CHANNEL_L].size()); }
};

FBDynamicAudioBlock::
FBDynamicAudioBlock(int capacity):
_store()
{
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
    _store[ch].reserve(capacity);
}

void 
FBDynamicAudioBlock::AppendFrom(FBFixedAudioBlock const& fixed)
{
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
    for (int s = 0; s < FB_FIXED_BLOCK_SIZE; s++)
      _store[ch].push_back(fixed[ch][s]);
}

void
FBDynamicAudioBlock::AppendFrom(FBRawAudioBlockView const& raw)
{
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
    for (int s = 0; s < raw.Count(); s++)
      _store[ch].push_back(raw._store[ch][s]);
}

void 
FBDynamicAudioBlock::MoveOneRawBlockToAndPad(FBRawAudioBlockView& raw)
{
  int samplesUsed = std::min(raw.Count(), Count());
  int samplesPadded = std::max(0, raw.Count() - samplesUsed);
  raw.SetToZero(0, samplesPadded);
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
  {
    for (int s = samplesPadded; s < samplesPadded + samplesUsed; s++)
      raw._store[ch][s] = _store[ch][s - samplesPadded];
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + samplesUsed);
  }
}

void
FBDynamicAudioBlock::MoveOneFixedBlockTo(FBFixedAudioBlock& fixed)
{
  assert(_store[0].size() >= FB_FIXED_BLOCK_SIZE);
  for (int ch = 0; ch < FB_CHANNELS_STEREO; ch++)
  {
    for (int s = 0; s < FB_FIXED_BLOCK_SIZE; s++)
      fixed[ch][s] = _store[ch][s];
    _store[ch].erase(_store[ch].begin(), _store[ch].begin() + FB_FIXED_BLOCK_SIZE);
  }
}

// todo noinline