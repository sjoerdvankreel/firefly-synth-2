#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/base/shared/FBObjectLifetime.hpp>
#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

template <class Derived>
class FBAutomationRamper:
public FBFixedBlockProcessor<FBAutomationRamper<Derived>>
{
  FBAccAddrsBase* const _acc;
  FBScalarAddrsBase* const _scalar;

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
  FBAccAddrsBase* acc,
  FBScalarAddrsBase* scalar,
  int maxHostSampleCount) :
FBFixedBlockProcessor<FBAutomationRamper<Derived>>(scalar, maxHostSampleCount),
_acc(acc),
_scalar(scalar) {}

template <class Derived> 
void FBAutomationRamper<Derived>::ProcessFixed(
  FBPlugInputBlock const& input, FBPlugAudioBlock& output)
{
  for (int ap = 0; ap < _denseAddrs->pos.size(); ap++)
    *_denseAddrs->pos[ap] = 0;
  for (int ap = 0; ap < _scalarAddrs->acc.size(); ap++)
    _denseAddrs->cv[ap]->Fill(0, _denseAddrs->cv[ap]->Count(), *_scalarAddrs->acc[ap]);

  auto& accEvents = input.events.acc;
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
      (*_denseAddrs->cv[event.index])[currentPos + pos] = 
        currentVal + pos / static_cast<float>(posRange) * valRange;
    if (ae < accEvents.size() - 1 && accEvents[ae + 1].index != event.index)
      for (int pos = event.position; pos < input.audio.Count(); pos++)
        (*_denseAddrs->cv[event.index])[pos] = event.normalized;
  }

  static_cast<Derived*>(this)->ProcessAutomation(input, output);
}