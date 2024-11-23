# pragma once

#include <playground_plug/base/shared/FFHostBlock.hpp>
#include <playground_plug/base/shared/FFSignalBlock.hpp>

class FFOutputAggregator
{
  FFAccumulatedOutputBlock _accumulated;

public:
  FFOutputAggregator(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFOutputAggregator);

  void Aggregate(FFRawStereoBlock& output);
  void Accumulate(FFFixedStereoBlock const& input);
};