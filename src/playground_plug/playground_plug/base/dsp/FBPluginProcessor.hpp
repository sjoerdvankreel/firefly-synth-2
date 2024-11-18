#pragma once

#include <playground_plug/base/dsp/FBHostBlock.hpp>
#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>
#include <cassert>

// handles fixed block sizes, parameter smoothing, essentially all 
// the boring tech stuff. ProcessPluginBlock is where the interesting 
// stuff happens, so it can deal with fixed-size buffers only. 
// also i don't know how to do this without crtp and no virtuals.
template <class Derived, class ProcessorMemory>
class FBPluginProcessor
{
  int _accumulated = 0;
  float const _sampleRate;

  // for dealing with fixed size buffers
  FBHostBlock _rollingHostBlock;
  int _currentRollingWindowSize = 0;

public:
  void ProcessHostBlock(FBHostBlock& hostBlock);

protected:
  ProcessorMemory _memory = {};
  FBRuntimeTopo<ProcessorMemory> const* const _topo;
  FBPluginProcessor(FBRuntimeTopo<ProcessorMemory> const* topo, int maxSampleCount, float sampleRate);
};

template <class Derived, class ProcessorMemory>
FBPluginProcessor<Derived, ProcessorMemory>::
FBPluginProcessor(FBRuntimeTopo<ProcessorMemory> const* topo, int maxSampleCount, float sampleRate):
_topo(topo),
_sampleRate(sampleRate),
_rollingHostBlock(maxSampleCount)
{
}

template <class Derived, class ProcessorMemory> void
FBPluginProcessor<Derived, ProcessorMemory>::ProcessHostBlock(FBHostBlock& hostBlock)
{
  // TODO debug the heck out of this stuff
  // TODO deal with non-accurate
  
  // gather audio in
  for (int hostSample = 0; hostSample < hostBlock.currentSampleCount; hostSample++)
    for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      _rollingHostBlock.audioIn[channel].push_back(hostBlock.audioIn[channel][hostSample]);

  // gather note events
  for (int i = 0; i < hostBlock.noteEvents.size(); i++)
  {
    FBNoteEvent noteEvent = hostBlock.noteEvents[i];
    noteEvent.position += _currentRollingWindowSize;
    _rollingHostBlock.noteEvents.push_back(noteEvent);
  }

  // gather automation events (sample accurate)
  for (int i = 0; i < hostBlock.autoEvents.size(); i++)
  {
    FBAutoEvent autoEvent = hostBlock.autoEvents[i];
    autoEvent.position += _currentRollingWindowSize;
    _rollingHostBlock.autoEvents.push_back(autoEvent);
  }

  _currentRollingWindowSize += hostBlock.currentSampleCount;

  while (_currentRollingWindowSize >= ProcessorMemory::BlockSize)
  {
    _currentRollingWindowSize -= ProcessorMemory::BlockSize;
  }


#if 0
  // handle leftover from the previous round
  int samplesProcessed = 0;
  for (int s = 0; s < hostBlock.currentSampleCount && s < _remainingOut[FB_CHANNEL_L].size(); s++)
  {
    for(int channel = 0; channel < 2; channel++)
      hostBlock.audioOut[channel][samplesProcessed] = _remainingOut[channel][s];
    samplesProcessed++;
  }

  // delete processed leftover from the remaining buffer
  for (int channel = 0; channel < 2; channel++)
    _remainingOut[channel].erase(
      _remainingOut[channel].begin(),
      _remainingOut[channel].begin() + samplesProcessed);

  // non-automatable parameters only changed by ui, no need to split blocks 
  // (actually there's no sample position anyway)
  for (int pe = 0; pe < hostBlock.plugEvents.size(); pe++)
  {
    auto const& event = hostBlock.plugEvents[pe];
    int index = _topo->tagToPlugParam.at(event.tag);
    auto const& param = _topo->plugParams[index];
    *(_topo->plugParams[index].staticTopo.plugParamAddr(param.moduleSlot, param.paramSlot, &_memory)) = event.normalized;
  }

  FBProcessorContext context;
  context.moduleSlot = -1;
  context.sampleRate = _sampleRate;

  // deal with remainder of host block
  while (samplesProcessed < hostBlock.currentSampleCount)
  {
    static_cast<Derived*>(this)->ProcessPluginBlock(context);

    // process in chunks of internal block size, may cause leftovers
    int blockSample = 0;
    for (; blockSample < ProcessorMemory::BlockSize && samplesProcessed < hostBlock.currentSampleCount; blockSample++)
    {
      for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
        hostBlock.audioOut[channel][samplesProcessed] = _memory.masterOut[channel][blockSample];
      samplesProcessed++;
    }

    // TODO its not even possible to overshoot when counting in input, so necessarily introduce latency?
    // if we overshoot, stick it in the remaining buffer
    for (; blockSample < ProcessorMemory::BlockSize; blockSample++)
    {
      for (int channel = 0; channel < 2; channel++)
        _remainingOut[channel].push_back(_memory.masterOut[channel][blockSample]);
      samplesProcessed++;
      assert(_remainingOut[FB_CHANNEL_L].size() <= _maxRemaining);
    }
  }
#endif
}