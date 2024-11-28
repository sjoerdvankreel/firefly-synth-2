#pragma once

#include <playground_base/shared/FBObjectLifetime.hpp>
#include <playground_base/dsp/pipeline/FBPipelineAudioBlock.hpp>

#include <cassert>
#include <array>
#include <vector>

class FBPipelineAudioBlock;

class FBHostAudioBlock
{
  int _count;
  std::array<float*, 2> _store;
  friend class FBPipelineAudioBlock;

public:
  FB_COPY_MOVE_DEFCTOR(FBHostAudioBlock);
  FBHostAudioBlock(float* l, float* r, int count) :
  _count(count), _store({ l, r }) {}

  float const* operator[](int channel) const { return _store[channel]; }

  void Fill(int from, int to, float val)
  {
    assert(0 <= from && from <= to && to <= _count);
    for (int ch = 0; ch < 2; ch++)
      for (int s = from; s < to; s++)
        _store[ch][s] = val;
  }

  void CopyFrom(FBPipelineAudioBlock const& pipeline, int srcOffset, int count);
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