#pragma once

#include <playground_base/dsp/shared/FBAnyAudioBlock.hpp>
#include <playground_base/base/shared/FBObjectLifetime.hpp>

#include <array>
#include <vector>

class FBPlugAudioBlock;
class FBHostAudioBlock;

class FBPipelineAudioBlock:
public FBAnyAudioBlock<FBPipelineAudioBlock>
{
  std::array<std::vector<float>, 2> _store = {};

public:
  void Drop(int count);
  void Append(FBPlugAudioBlock const& plug);
  void Append(FBHostAudioBlock const& host);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPipelineAudioBlock);

  std::vector<float> const& 
  operator[](int channel) const { return _store[channel]; }
  int Count() const { return static_cast<int>(_store[0].size()); }
};