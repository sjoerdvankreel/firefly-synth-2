# pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>

class FFOutputAggregator
{
  FFAccumulatedOutputBlock _accumulated;

public:
  FFOutputAggregator(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFOutputAggregator);

  void Aggregate(FFRawStereoBlockView& output);
  void Accumulate(FFFixedStereoBlock const& input);
};