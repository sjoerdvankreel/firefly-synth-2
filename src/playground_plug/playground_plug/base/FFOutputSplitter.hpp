# pragma once

#include <playground_plug/base/shared/FFHostBlock.hpp>
#include <playground_plug/base/shared/FFSignalBlock.hpp>

class FFOutputSplitter
{
  FFAccumulatedOutputBlock _accumulated;

public:
  FFOutputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFOutputSplitter);

  void RemoveSamples(int count);
  void AccumulateFixedBlock(FFFixedStereoBlock const& output);
  FFAccumulatedOutputBlock const& GetAccumulatedBlock() const;
};