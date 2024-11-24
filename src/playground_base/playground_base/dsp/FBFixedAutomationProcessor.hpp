#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

template <class Derived>
class FBFixedAutomationProcessor:
public FBFixedBlockProcessor<FBFixedAutomationProcessor<Derived>>
{
  FBScalarParamMemoryBase* const _scalarMemory;
  FBProcessorParamMemoryBase* const _processorMemory;

public:
  FBFixedAutomationProcessor(
    FBScalarParamMemoryBase* scalarMemory, 
    FBProcessorParamMemoryBase* processorMemory,
    int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBFixedAutomationProcessor);
  void ProcessFixed(FBFixedInputBlock const& input, FBFixedStereoBlock& output);
};

template <class Derived>
FBFixedAutomationProcessor<Derived>::
FBFixedAutomationProcessor(
  FBScalarParamMemoryBase* scalarMemory,
  FBProcessorParamMemoryBase* processorMemory,
  int maxHostSampleCount) :
FBFixedBlockProcessor<FBFixedAutomationProcessor<Derived>>(maxHostSampleCount),
_scalarMemory(scalarMemory),
_processorMemory(processorMemory) {}

template <class Derived> void 
FBFixedAutomationProcessor<Derived>::ProcessFixed(
  FBFixedInputBlock const& input, FBFixedStereoBlock& output)
{
  for (auto const& be : input.events.blockParam)
    *_scalarMemory->blockAddr[be.index] = be.normalized;
  static_cast<Derived*>(this)->ProcessAutomation(input, output);
}