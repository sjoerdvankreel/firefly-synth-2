#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

template <class Derived>
class FBAutomationRamper:
public FBFixedBlockProcessor<FBAutomationRamper<Derived>>
{
  FBScalarParamMemoryBase* const _scalarMemory;
  FBProcessorParamMemoryBase* const _processorMemory;

  void RampAccEvents(
    std::vector<FBAccParamEvent> const& accEvents);

public:
  FBAutomationRamper(
    FBScalarParamMemoryBase* scalarMemory, 
    FBProcessorParamMemoryBase* processorMemory,
    int maxHostSampleCount);  
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAutomationRamper);

  void ProcessFixed(FBFixedInputBlock const& input, FBFixedAudioBlock& output);
};

template <class Derived>
FBAutomationRamper<Derived>::
FBAutomationRamper(
  FBScalarParamMemoryBase* scalarMemory,
  FBProcessorParamMemoryBase* processorMemory,
  int maxHostSampleCount) :
FBFixedBlockProcessor<FBAutomationRamper<Derived>>(maxHostSampleCount),
_scalarMemory(scalarMemory),
_processorMemory(processorMemory) {}

template <class Derived> 
void FBAutomationRamper<Derived>::ProcessFixed(
  FBFixedInputBlock const& input, FBFixedAudioBlock& output)
{
  for (auto const& be : input.events.blockParam)
    *_scalarMemory->blockAddr[be.index] = be.normalized;
  for (int ap = 0; ap < _processorMemory->posAddr.size(); ap++)
    *_processorMemory->posAddr[ap] = 0;

  RampAccEvents(input.events.accParam);
  static_cast<Derived*>(this)->ProcessAutomation(input, output);
}

template <class Derived> void
FBAutomationRamper<Derived>::RampAccEvents(
  std::vector<FBAccParamEvent> const& accEvents)
{
  for (int ae = 0; ae < _scalarMemory->accAddr.size(); ae++)
    _processorMemory->denseAddr[ae]->Fill(*_scalarMemory->accAddr[ae]);

  for (int ae = 0; ae < accEvents.size(); ae++)
  {
    auto const& event = accEvents[ae];
    float currentVal = *_scalarMemory->accAddr[event.index];
    int currentPos = *_processorMemory->posAddr[event.index];
    int posRange = event.position - currentPos;
    float valRange = event.normalized - currentVal;
    *_scalarMemory->accAddr[event.index] = event.normalized;
    *_processorMemory->posAddr[event.index] = event.position;

    for (int p = 0; p <= posRange; p++)
      (*_processorMemory->denseAddr[event.index])[currentPos + p]
        = currentVal + p / static_cast<float>(posRange) * valRange;
    if (ae < accEvents.size() - 1 && accEvents[ae + 1].index != event.index)
      for (int p = event.position; p < FB_FIXED_BLOCK_SIZE; p++)
        (*_processorMemory->denseAddr[event.index])[p] = event.normalized;
  }
}