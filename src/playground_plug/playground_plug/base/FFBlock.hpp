#pragma once

#include <playground_plug/base/FFConfig.hpp>
#include <playground_plug/base/FFUtility.hpp>

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
  { for (int i = 0; i < Store().size(); i++) rhs[i] = (*this)[i]; }

  void InPlaceMultiply(float x) 
  { for (int i = 0; i < Store().size(); i++) (*this)[i] *= x; }
  void InPlaceAdd(DerivedT const& rhs)
  { for (int i = 0; i < Store().size(); i++) (*this)[i] += rhs[i]; }
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
