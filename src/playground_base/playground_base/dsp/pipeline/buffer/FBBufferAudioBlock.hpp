#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>

#include <array>
#include <vector>

class FBHostAudioBlock;

class FBBufferAudioBlock final
{
  std::array<std::vector<float>, 2> _store = {};

public:
  void Drop(int count);
  void AppendHost(FBHostAudioBlock const& rhs);
  void AppendFixed(FBFixedFloatAudioArray const& rhs);

  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferAudioBlock);
  int Count() const { return static_cast<int>(_store[0].size()); }
  std::vector<float> const& operator[](int ch) const { return _store[ch]; }
};