#pragma once

#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/base/shared/FBObjectLifetime.hpp>
#include <playground_base/dsp/FBFixedBlockProcessor.hpp>

template <class Derived>
class FBAutomationRamper:
public FBFixedBlockProcessor<FBAutomationRamper<Derived>>
{
  FBProcAddrsBase* const _proc;
  FBScalarAddrsBase* const _scalar;

public:
  FBAutomationRamper(
    FBProcAddrsBase* proc,
    FBScalarAddrsBase* scalar,
    int maxHostSampleCount);  
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBAutomationRamper);

  void ProcessFixed(
    FBPlugInputBlock const& input, FBPlugAudioBlock& output);
};

template <class Derived>
FBAutomationRamper<Derived>::
FBAutomationRamper(
  FBProcAddrsBase* proc,
  FBScalarAddrsBase* scalar,
  int maxHostSampleCount) :
FBFixedBlockProcessor<FBAutomationRamper<Derived>>(scalar, maxHostSampleCount),
_proc(proc),
_scalar(scalar) {}

template <class Derived> 
void FBAutomationRamper<Derived>::ProcessFixed(
  FBPlugInputBlock const& input, FBPlugAudioBlock& output)
{
  for (int p = 0; p < _proc->pos.size(); p++)
    *_proc->pos[p] = 0;
  for (int c = 0; c < _proc->cv.size(); c++)
    _proc->cv[c]->Fill(0, _proc->cv[c]->Count(), *_scalar->acc[c]);

  for (int a = 0; a < input.acc.size(); a++)
  {
    auto const& event = input.acc[a];
    int currentPos = *_proc->pos[event.index];
    float currentVal = *_scalar->acc[event.index];
    int posRange = event.pos - currentPos;
    float valRange = event.normalized - currentVal;
    *_proc->pos[event.index] = event.pos;
    *_scalar->acc[event.index] = event.normalized;

    for (int pos = 0; pos <= posRange; pos++)
      (*_proc->cv[event.index])[currentPos + pos] =
        currentVal + pos / static_cast<float>(posRange) * valRange;
    if (a < input.acc.size() - 1 && input.acc[a + 1].index != event.index)
      for (int pos = event.pos; pos < input.audio.Count(); pos++)
        (*_proc->cv[event.index])[pos] = event.normalized;
  }

  static_cast<Derived*>(this)->ProcessAutomation(input, output);
}