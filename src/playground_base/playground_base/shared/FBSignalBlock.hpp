#pragma once

#include <playground_base/shared/FBSharedUtility.hpp>

#include <array>
#include <vector>
#include <cassert>
#include <cstdint>

#define FB_CHANNEL_L 0
#define FB_CHANNEL_R 1
#define FB_CHANNELS_STEREO 2
#define FB_FIXED_BLOCK_SIZE 16

class FBRawBlockView
{
  float* _store;
  std::size_t _size;

public:
  FB_COPY_MOVE_DEFCTOR(FBRawBlockView);
  FBRawBlockView(float* store, int count) :
  _size(count), _store(store) {}

  float* begin()
  { return _store; }
  float* end()
  { return _store + _size; }

  float const* begin() const
  { return _store; }
  float const* end() const
  { return _store + _size; }

  std::size_t size() const 
  { return _size; }
  void fill(float val)
  { std::fill(_store, _store + _size, val); }

  float& operator[](int index) 
  { assert(0 <= index && index < _size); return _store[index]; }
  float const& operator[](int index) const 
  { assert(0 <= index && index < _size); return _store[index]; }
};

template <class DerivedT>
class FBMonoBlockMixin
{
  auto& Store() 
  { return static_cast<DerivedT*>(this)->_store; }
  auto const& Store() const 
  { return static_cast<DerivedT const*>(this)->_store; }

public:
  float& operator[](int index) 
  { return Store()[index]; }
  float const operator[](int index) const 
  { return Store()[index]; }
  int Count() const
  { return static_cast<int>(Store().size()); }

  void CopyTo(DerivedT& rhs) const
  { for (int i = 0; i < Count(); i++) rhs[i] = (*this)[i]; }
  void Fill(int from, int to, float val) 
  { std::fill(Store().begin() + from, Store().begin() + to, val); }

  void InPlaceMultiply(float x) 
  { for (int i = 0; i < Count(); i++) (*this)[i] *= x; }
  void InPlaceAdd(DerivedT const& rhs) 
  { for (int i = 0; i < Count(); i++) (*this)[i] += rhs[i]; }
  void ShiftLeft(int count)
  { for (int i = 0; i < count - 1; i++) (*this)[i] = (*this)[i + 1]; }

  template <class T>
  void CopyTo(T& rhs, int srcOffset, int tgtOffset, int count) const
  { for (int i = 0; i < count; i++) rhs[tgtOffset + i] = (*this)[srcOffset + i]; }
};

class FBRawMonoBlockView:
public FBMonoBlockMixin<FBRawMonoBlockView>
{
  FBRawBlockView _store;
  friend class FBMonoBlockMixin<FBRawMonoBlockView>;

public:
  FB_COPY_MOVE_DEFCTOR(FBRawMonoBlockView);
  FBRawMonoBlockView(float* store, int count) :
  _store(store, count) {}
};

class FBDynamicMonoBlock:
public FBMonoBlockMixin<FBDynamicMonoBlock>
{
  std::vector<float> _store;
  friend class FBMonoBlockMixin<FBDynamicMonoBlock>;

public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBDynamicMonoBlock);
  FBDynamicMonoBlock(int count):
  _store(count, 0.0f) {}

  FBRawMonoBlockView GetRawBlockView()
  { return FBRawMonoBlockView(_store.data(), static_cast<int>(_store.size())); }
};

class alignas(FB_FIXED_BLOCK_SIZE * sizeof(float)) FBFixedMonoBlock:
public FBMonoBlockMixin<FBFixedMonoBlock>
{
  std::array<float, FB_FIXED_BLOCK_SIZE> _store;
  friend class FBMonoBlockMixin<FBFixedMonoBlock>;

public:
  FB_NOCOPY_MOVE_DEFCTOR(FBFixedMonoBlock);
};

template <class DerivedT>
class FBStereoBlockMixin
{
  auto& Store() 
  { return static_cast<DerivedT*>(this)->_store; }
  auto const& Store() const 
  { return static_cast<DerivedT const*>(this)->_store; }

public:
  int Count() const
  { return Store()[0].Count(); }
  auto& operator[](int channel) 
  { return Store()[channel]; }
  auto const& operator[](int channel) const
  { return Store()[channel]; }

  void CopyTo(DerivedT& rhs) const
  { for(int ch = 0; ch < FB_CHANNELS_STEREO; ch++) (*this)[ch].CopyTo(rhs[ch]); }
  void Fill(int from, int to, float val) 
  { for(int ch = 0; ch < FB_CHANNELS_STEREO; ch++)  (*this)[ch].Fill(from, to, val); }

  void ShiftLeft(int count)
  { for(int ch = 0; ch < FB_CHANNELS_STEREO; ch++) (*this)[ch].ShiftLeft(count); }
  void InPlaceMultiply(float x) 
  { for(int ch = 0; ch < FB_CHANNELS_STEREO; ch++) (*this)[ch].InPlaceMultiply(x); }
  void InPlaceAdd(DerivedT const& rhs)
  { for(int ch = 0; ch < FB_CHANNELS_STEREO; ch++) (*this)[ch].InPlaceAdd(rhs[ch]); }

  template <class T>
  void CopyTo(T& rhs, int srcOffset, int tgtOffset, int count) const
  { for(int ch = 0; ch < FB_CHANNELS_STEREO; ch++) (*this)[ch].CopyTo(rhs[ch], srcOffset, tgtOffset, count); }
};

class FBRawStereoBlockView:
public FBStereoBlockMixin<FBRawStereoBlockView>
{
  std::array<FBRawMonoBlockView, FB_CHANNELS_STEREO> _store;
  friend class FBStereoBlockMixin<FBRawStereoBlockView>;

public:
  FB_COPY_MOVE_DEFCTOR(FBRawStereoBlockView);
  FBRawStereoBlockView(float* l, float* r, int count) :
  _store({ FBRawMonoBlockView(l, count), FBRawMonoBlockView(r, count) }) {}
  FBRawStereoBlockView(FBRawMonoBlockView const& l, FBRawMonoBlockView const& r) :
  _store({ l, r }) {}
};

class alignas(FB_FIXED_BLOCK_SIZE * FB_CHANNELS_STEREO * sizeof(float)) FFFixedStereoBlock:
public FBStereoBlockMixin<FFFixedStereoBlock>
{
  std::array<FBFixedMonoBlock, FB_CHANNELS_STEREO> _store;
  friend class FBStereoBlockMixin<FFFixedStereoBlock>;

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFFixedStereoBlock);
};

class FFDynamicStereoBlock:
public FBStereoBlockMixin<FFDynamicStereoBlock>
{
  std::array<FBDynamicMonoBlock, FB_CHANNELS_STEREO> _store;
  friend class FBStereoBlockMixin<FFDynamicStereoBlock>;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFDynamicStereoBlock);
  FFDynamicStereoBlock(int count) :
  _store({ FBDynamicMonoBlock(count), FBDynamicMonoBlock(count) }) {}

  FBRawStereoBlockView GetRawBlockView()
  { return FBRawStereoBlockView(_store[FB_CHANNEL_L].GetRawBlockView(), _store[FB_CHANNEL_R].GetRawBlockView()); }
};