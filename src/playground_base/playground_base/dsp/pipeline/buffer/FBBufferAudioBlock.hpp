#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>
#include <vector>

class FBHostAudioBlock;
class FBFixedAudioBlock;

class FBBufferAudioBlock final
{
  std::array<std::vector<float>, 2> _store = {};

public:
  void Drop(int count);
  void Append(FBHostAudioBlock const& rhs);
  void Append(FBFixedAudioBlock const& rhs);

  FB_NOCOPY_NOMOVE_DEFCTOR(FBBufferAudioBlock);
  int Count() const { return static_cast<int>(_store[0].size()); }
  std::vector<float> const& operator[](int ch) const { return _store[ch]; }
};