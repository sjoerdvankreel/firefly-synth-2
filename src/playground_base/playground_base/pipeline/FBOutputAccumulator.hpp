#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBHostAudioBlock;

class FBOutputAccumulator
{
  bool _hitFixedBlockSize;
  FBDynamicAudioBlock _accumulated;

public:
  FBOutputAccumulator(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBOutputAccumulator);

  void AggregateTo(FBHostAudioBlock& output);
  void AccumulateFrom(FBFixedAudioBlock const& input);
};