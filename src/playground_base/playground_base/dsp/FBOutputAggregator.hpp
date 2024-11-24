# pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>

class FBOutputAggregator
{
  FBAccumulatedOutputBlock _accumulated;

public:
  FBOutputAggregator(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBOutputAggregator);

  void Aggregate(FFRawStereoBlockView& output);
  void Accumulate(FFFixedStereoBlock const& input);
};