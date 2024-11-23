#pragma once

#include <playground_plug/base/FFUtility.hpp>
#include <playground_plug/base/FFDSPConfig.hpp>

#include <array>
#include <vector>
#include <cassert>
#include <cstdint>

class FFRawBlock
{
  float* const _store;
  std::size_t const _size;

public:
  FF_NOCOPY_MOVE_NODEFCTOR(FFRawBlock);
  FFRawBlock(float* store, int count) :
  _size(count), _store(store) {}

  float& operator[](int index) 
  { assert(0 <= index && index < _size); 
    return _store[index]; }
  float const& operator[](int index) const 
  { assert(0 <= index && index < _size); 
    return _store[index]; }

  std::size_t size() const 
  { return _size; }
  void fill(float val)
  { std::fill(_store, _store + _size, val); }
};

template <class DerivedT>
class FFMonoBlockMixin
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

  void SetToZero() 
  { Store().fill(0.0f); }
  void CopyTo(DerivedT& rhs) const
  { for (int i = 0; i < Count(); i++)
    rhs[i] = (*this)[i]; }

  void ShiftLeft(int count)
  { for (int i = 0; i < count; i++)
      (*this)[i] = (*this)[i + count]; }
  void InPlaceMultiply(float x) 
  { for (int i = 0; i < Count(); i++)
    (*this)[i] *= x; }
  void InPlaceAdd(DerivedT const& rhs)
  { for (int i = 0; i < Count(); i++)
    (*this)[i] += rhs[i]; }

  template <class T>
  void CopyTo(T& rhs, int srcOffset, int tgtOffset, int count) const
  { assert(srcOffset + count <= Count());
    for (int i = 0; i < count; i++)
      rhs[tgtOffset + i] = (*this)[srcOffset + i]; }
};

class FFRawMonoBlock:
public FFMonoBlockMixin<FFRawMonoBlock>
{
  FFRawBlock _store;
  friend class FFMonoBlockMixin<FFRawMonoBlock>;

public:
  FF_NOCOPY_MOVE_NODEFCTOR(FFRawMonoBlock);
  FFRawMonoBlock(float* store, int count) : 
  _store(store, count) {}
};

class FFDynamicMonoBlock:
public FFMonoBlockMixin<FFDynamicMonoBlock>
{
  std::vector<float> _store;
  friend class FFMonoBlockMixin<FFDynamicMonoBlock>;

public:
  FF_NOCOPY_MOVE_NODEFCTOR(FFDynamicMonoBlock);
  FFDynamicMonoBlock(int count): _store(count, 0.0f) {}
};

class alignas(FF_BLOCK_SIZE * sizeof(float)) FFFixedMonoBlock:
public FFMonoBlockMixin<FFFixedMonoBlock>
{
  std::array<float, FF_BLOCK_SIZE> _store;
  friend class FFMonoBlockMixin<FFFixedMonoBlock>;

public:
  FF_NOCOPY_MOVE_DEFCTOR(FFFixedMonoBlock);
};

template <class DerivedT>
class FFStereoBlockMixin
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

  void SetToZero() 
  { for(int ch = 0; ch < FF_CHANNELS_STEREO; ch++) 
    (*this)[ch].SetToZero(); }
  void CopyTo(DerivedT& rhs) const
  { for(int ch = 0; ch < FF_CHANNELS_STEREO; ch++) 
    (*this)[ch].CopyTo(rhs[ch]); }

  void ShiftLeft(int count)
  { for(int ch = 0; ch < FF_CHANNELS_STEREO; ch++) 
    (*this)[ch].ShiftLeft(count); }
  void InPlaceMultiply(float x) 
  { for(int ch = 0; ch < FF_CHANNELS_STEREO; ch++) 
    (*this)[ch].InPlaceMultiply(x); }
  void InPlaceAdd(DerivedT const& rhs)
  { for(int ch = 0; ch < FF_CHANNELS_STEREO; ch++) 
    (*this)[ch].InPlaceAdd(rhs[ch]); }

  template <class T>
  void CopyTo(T& rhs, int srcOffset, int tgtOffset, int count) const
  { for(int ch = 0; ch < FF_CHANNELS_STEREO; ch++) 
    (*this)[ch].CopyTo(rhs[ch], srcOffset, tgtOffset, count); }
};

class FFRawStereoBlock:
public FFStereoBlockMixin<FFRawStereoBlock>
{
  std::array<FFRawMonoBlock, FF_CHANNELS_STEREO> _store;
  friend class FFStereoBlockMixin<FFRawStereoBlock>;

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFRawStereoBlock);
  FFRawStereoBlock(float* l, float* r, int count) :
  _store({ FFRawMonoBlock(l, count), FFRawMonoBlock(r, count) }) {}
};

class FFDynamicStereoBlock:
public FFStereoBlockMixin<FFDynamicStereoBlock>
{
  std::array<FFDynamicMonoBlock, FF_CHANNELS_STEREO> _store;
  friend class FFStereoBlockMixin<FFDynamicStereoBlock>;

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFDynamicStereoBlock);
  FFDynamicStereoBlock(int count) :
  _store({ FFDynamicMonoBlock(count), FFDynamicMonoBlock(count) }) {}
};

class alignas(FF_BLOCK_SIZE * FF_CHANNELS_STEREO * sizeof(float)) FFFixedStereoBlock:
public FFStereoBlockMixin<FFFixedStereoBlock>
{
  std::array<FFFixedMonoBlock, FF_CHANNELS_STEREO> _store;
  friend class FFStereoBlockMixin<FFFixedStereoBlock>;

public:
  FF_NOCOPY_NOMOVE_DEFCTOR(FFFixedStereoBlock);
};