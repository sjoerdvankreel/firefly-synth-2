#pragma once

#include <playground_plug/base/FFUtility.hpp>
#include <playground_plug/base/FFDSPConfig.hpp>

#include <array>
#include <vector>

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

  void SetToZero() 
  { Store().fill(0.0f); }
  void CopyTo(DerivedT& rhs) const
  { for (int i = 0; i < Store().size(); i++) 
    rhs[i] = (*this)[i]; }

  void ShiftLeft(int count)
  { for (int i = 0; i < count; i++)
      (*this)[i] = (*this)[i + count]; }
  void InPlaceMultiply(float x) 
  { for (int i = 0; i < Store().size(); i++) 
    (*this)[i] *= x; }
  void InPlaceAdd(DerivedT const& rhs)
  { for (int i = 0; i < Store().size(); i++) 
    (*this)[i] += rhs[i]; }
};

class FFDynamicMonoBlock:
public FFMonoBlockMixin<FFDynamicMonoBlock>
{
  std::vector<float> _store;
  friend class FFMonoBlockMixin<FFDynamicMonoBlock>;

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFDynamicMonoBlock);
  FFDynamicMonoBlock(int size): _store(size, 0.0f) {}
};

class alignas(FF_BLOCK_SIZE * sizeof(float)) FFFixedMonoBlock:
public FFMonoBlockMixin<FFFixedMonoBlock>
{
  std::array<float, FF_BLOCK_SIZE> _store;
  friend class FFMonoBlockMixin<FFFixedMonoBlock>;

public:
  FF_NOCOPY_NOMOVE_DEFCTOR(FFFixedMonoBlock);
};

template <class DerivedT>
class FFStereoBlockMixin
{
  auto& Store() 
  { return static_cast<DerivedT*>(this)->_store; }
  auto const& Store() const 
  { return static_cast<DerivedT const*>(this)->_store; }

public:
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
};

class FFDynamicStereoBlock:
public FFStereoBlockMixin<FFDynamicStereoBlock>
{
  std::array<std::vector<float>, FF_CHANNELS_STEREO> _store;
  friend class FFStereoBlockMixin<FFDynamicStereoBlock>;

public:
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFDynamicStereoBlock);
  FFDynamicStereoBlock(int size) : 
  _store({ std::vector<float>(size, 0.0f), std::vector<float>(size, 0.0f) }) {}
};

class alignas(FF_BLOCK_SIZE * FF_CHANNELS_STEREO * sizeof(float)) FFFixedStereoBlock:
public FFStereoBlockMixin<FFFixedStereoBlock>
{
  std::array<std::array<float, FF_BLOCK_SIZE>, FF_CHANNELS_STEREO> _store;
  friend class FFStereoBlockMixin<FFFixedStereoBlock>;

public:
  FF_NOCOPY_NOMOVE_DEFCTOR(FFFixedStereoBlock);
};