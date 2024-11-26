# pragma once

#include <playground_base/shared/FBHostBlock.hpp>

#include <vector>

class FBRawAudioBlockView;

class FBOutputAggregator
{
  bool _hitFixedBlockSize;
  int _maxHostSampleCount;
  
  std::vector<float> _xl = {};
  std::vector<float> _xr = {};

public:
  FBOutputAggregator(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBOutputAggregator);

  void Aggregate(FBRawAudioBlockView& output);
  void Accumulate(FBFixedAudioBlock const& input);
};