#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <array>

class FBBufferAudioBlock;

class FBHostAudioBlock final
{
  int _count = 0;
  std::array<float*, 2> _store = {};

public:
  FB_COPY_MOVE_DEFCTOR(FBHostAudioBlock);
  FBHostAudioBlock(float** channels, int count);

  int Count() const { return _count; }
  void CopyFrom(FBBufferAudioBlock const& rhs, int count);
  float const* operator[](int ch) const { return _store[ch]; }
};