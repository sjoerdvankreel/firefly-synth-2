#pragma once

#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

template <class Derived>
class FBFixedAutomationProcessor:
public FBFixedBlockProcessor<FBFixedAutomationProcessor<Derived>>
{
public:
  FBFixedAutomationProcessor(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedAutomationProcessor);
  void ProcessFixed(FBFixedInputBlock const& input, FBFixedStereoBlock& output);
};

template <class Derived>
FBFixedAutomationProcessor<Derived>::
FBFixedAutomationProcessor(int maxHostSampleCount) :
FBFixedBlockProcessor<FBFixedAutomationProcessor<Derived>>(maxHostSampleCount) {}

template <class Derived> void 
FBFixedAutomationProcessor<Derived>::ProcessFixed(
  FBFixedInputBlock const& input, FBFixedStereoBlock& output)
{
  static_cast<Derived*>(this)->ProcessAutomation(input, output);
}