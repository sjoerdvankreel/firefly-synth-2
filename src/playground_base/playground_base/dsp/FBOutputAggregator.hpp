#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBRawAudioBlockView;

class FBOutputAggregator
{
  bool _hitFixedBlockSize;
  FBDynamicAudioBlock _accumulated;

public:
  FBOutputAggregator(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBOutputAggregator);

  void Aggregate(FBRawAudioBlockView& output);
  void Accumulate(FBFixedAudioBlock const& input);
};