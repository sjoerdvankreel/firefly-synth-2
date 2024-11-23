# pragma once

#include <playground_plug/base/FFDSPBlock.hpp>
#include <playground_plug/base/FFHostBlock.hpp>

class FFOutputSplitter
{
  FFAccumulatingOutputBlock _accumulating;

public:
  FFOutputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFOutputSplitter);

  void RemoveSamples(int count);
  FFAccumulatingOutputBlock const& GetAccumulatedBlock() const;
  void AccumulateFixedBlock(FFFixedStereoBlock const& audioOut);
};