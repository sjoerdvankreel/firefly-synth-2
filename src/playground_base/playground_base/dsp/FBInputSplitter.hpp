#pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <vector>

class FBInputSplitter
{
  FBFixedInputBlock _fixed;
  std::vector<int*>* const _posAddr;
  FBAccumulatedInputBlock _accumulated;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBInputSplitter);
  FBInputSplitter(std::vector<int*>* posAddr, int maxHostSampleCount);

  FBFixedInputBlock const* Split();
  void Accumulate(FBHostInputBlock const& input);
};