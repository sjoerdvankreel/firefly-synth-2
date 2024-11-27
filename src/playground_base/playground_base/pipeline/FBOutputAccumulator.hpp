#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBHostAudioBlock;

class FBOutputAccumulator
{
  bool _hitFixedBlockSize;
  FBPipelineAudioBlock _pipeline;

public:
  FBOutputAccumulator(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBOutputAccumulator);

  void AggregateTo(FBHostAudioBlock& output);
  void AccumulateFrom(FBPlugAudioBlock const& input);
};