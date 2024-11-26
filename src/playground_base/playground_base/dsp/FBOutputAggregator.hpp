# pragma once

#include <playground_base/shared/FBHostBlock.hpp>

#include <vector>

class FBRawStereoBlockView;

class FBOutputAggregator
{
  int _maxHostSampleCount;
  bool _hitMaxHostSampleCount;
  
  std::vector<float> _xl = {};
  std::vector<float> _xr = {};

public:
  FBOutputAggregator(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBOutputAggregator);

  void Aggregate(FBRawStereoBlockView& output);
  void Accumulate(FBFixedStereoBlock const& input);
};