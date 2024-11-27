#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/shared/FBObjectLifetime.hpp>
#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

template <class Derived>
class FBAutomationRamper:
public FBFixedBlockProcessor<FBAutomationRamper<Derived>>
{
  FBDenseParamAddrsBase* const _denseAddrs;
  FBScalarParamAddrsBase* const _scalarAddrs;

public:
  FBAutomationRamper(
    FBDenseParamAddrsBase* denseAddrs,
    FBScalarParamAddrsBase* scalarAddrs,
    int maxHostSampleCount);  
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAutomationRamper);

  void ProcessFixed(
    FBPlugInputBlock const& input, FBPlugAudioBlock& output);
};

template <class Derived>
FBAutomationRamper<Derived>::
FBAutomationRamper(
  FBDenseParamAddrsBase* denseAddrs,
  FBScalarParamAddrsBase* scalarAddrs,
  int maxHostSampleCount) :
FBFixedBlockProcessor<FBAutomationRamper<Derived>>(scalarAddrs, maxHostSampleCount),
_denseAddrs(denseAddrs),
_scalarAddrs(scalarAddrs) {}

template <class Derived> 
void FBAutomationRamper<Derived>::ProcessFixed(
  FBPlugInputBlock const& input, FBPlugAudioBlock& output)
{
  for (int ap = 0; ap < _denseAddrs->pos.size(); ap++)
    *_denseAddrs->pos[ap] = 0;
  for (int ap = 0; ap < _scalarAddrs->acc.size(); ap++)
    _denseAddrs->buffer[ap]->Fill(*_scalarAddrs->acc[ap]);

  auto& accEvents = input.events.accParam;
  for (int ae = 0; ae < accEvents.size(); ae++)
  {
    auto const& event = accEvents[ae];
    int currentPos = *_denseAddrs->pos[event.index];
    float currentVal = *_scalarAddrs->acc[event.index];
    int posRange = event.position - currentPos;
    float valRange = event.normalized - currentVal;
    *_denseAddrs->pos[event.index] = event.position;
    *_scalarAddrs->acc[event.index] = event.normalized;

    for (int pos = 0; pos <= posRange; pos++)
      (*_denseAddrs->buffer[event.index])[currentPos + pos] = 
        currentVal + pos / static_cast<float>(posRange) * valRange;
    if (ae < accEvents.size() - 1 && accEvents[ae + 1].index != event.index)
      for (int pos = event.position; pos < FB_PLUG_BLOCK_SIZE; pos++)
        (*_denseAddrs->buffer[event.index])[pos] = event.normalized;
  }

  static_cast<Derived*>(this)->ProcessAutomation(input, output);
}