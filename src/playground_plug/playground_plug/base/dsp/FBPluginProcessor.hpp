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
  float const _sampleRate;

  // for dealing with fixed size buffers
  FBHostBlock _accumulated;
  int _accumulatedInputSampleCount = 0;
  int _accumulatedOutputSampleCount = 0;

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
_accumulated(maxSampleCount)
{
  for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
  {
    _accumulated.audioIn[channel].reserve(maxSampleCount);
    _accumulated.audioOut[channel].reserve(maxSampleCount);
  }
}

template <class Derived, class ProcessorMemory> void
FBPluginProcessor<Derived, ProcessorMemory>::ProcessHostBlock(FBHostBlock& hostBlock)
{
  // TODO debug the heck out of this stuff
  // TODO deal with non-accurate

  // non-automatable parameters only changed by ui, no need to split blocks 
  // (actually there's no sample position anyway)
  for (int pe = 0; pe < hostBlock.plugEvents.size(); pe++)
  {
    auto const& event = hostBlock.plugEvents[pe];
    auto const& runtimeParam = _topo->GetRuntimeParamByTag(event.tag);
    *runtimeParam.PlugParamAddr(&_memory) = event.normalized;
  }

  // now proceed with the rolling window / fixed block size

  // gather audio in
  for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
    for (int hostSample = 0; hostSample < hostBlock.currentSampleCount; hostSample++)
      _accumulated.audioIn[channel].push_back(hostBlock.audioIn[channel][hostSample]);

  // gather note events
  for (int i = 0; i < hostBlock.noteEvents.size(); i++)
  {
    FBNoteEvent noteEvent = hostBlock.noteEvents[i];
    noteEvent.position += _accumulatedInputSampleCount;
    _accumulated.noteEvents.push_back(noteEvent);
  }

  // gather automation events (sample accurate)
  for (int i = 0; i < hostBlock.autoEvents.size(); i++)
  {
    FBAutoEvent autoEvent = hostBlock.autoEvents[i];
    autoEvent.position += _accumulatedInputSampleCount;
    _accumulated.autoEvents.push_back(autoEvent);
  }

  _accumulatedInputSampleCount += hostBlock.currentSampleCount;

  // we can now produce whatever multiple of internal 
  // block size fits in _accumulatedInputSampleCount
  FBProcessorContext context;
  context.moduleSlot = -1;
  context.sampleRate = _sampleRate;
  while (_accumulatedInputSampleCount >= ProcessorMemory::BlockSize)
  {
    // TODO lerp or filter or both?
    // construct dense buffers from time-stamped automation events
    // this is where the sort-by-sample first, param second comes in handy
    int eventIndex = 0;
    for (int sample = 0; sample < ProcessorMemory::BlockSize; sample++)
    {
      // update the static state
      for (; eventIndex < _accumulated.autoEvents.size() &&
        _accumulated.autoEvents[eventIndex].position == sample;
        eventIndex++)
      {
        auto const& event = hostBlock.autoEvents[eventIndex];
        auto const& runtimeParam = _topo->GetRuntimeParamByTag(event.tag);
        *runtimeParam.ScalarAutoParamAddr(&_memory) = event.normalized;
      }

      // for now just keep on pushing static state to dense buffers (so stair-stepping)
      for (int ap = 0; ap < _topo->autoParams.size(); ap++)
      {
        auto const& runtimeParam = _topo->autoParams[ap];
        (*runtimeParam.DenseAutoParamAddr(&_memory))[sample] = *runtimeParam.ScalarAutoParamAddr(&_memory);
      }
    }

    // run one round of internal block size and add to accumulated output
    static_cast<Derived*>(this)->ProcessPluginBlock(context);
    for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      _accumulated.audioOut[channel].insert(
        _accumulated.audioOut[channel].end(),
        _memory.masterOut[channel].store.begin(),
        _memory.masterOut[channel].store.end());
    _accumulatedOutputSampleCount += ProcessorMemory::BlockSize;

    // now subtract from accumulated input
    for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      _accumulated.audioIn[channel].erase(
        _accumulated.audioIn[channel].begin(),
        _accumulated.audioIn[channel].begin() + ProcessorMemory::BlockSize);
    std::erase_if(_accumulated.autoEvents, [](auto const& e) { return e.position < ProcessorMemory::BlockSize; });
    for (int i = 0; i < _accumulated.autoEvents.size(); i++)
      _accumulated.autoEvents[i].position -= ProcessorMemory::BlockSize;
    std::erase_if(_accumulated.noteEvents, [](auto const& e) { return e.position < ProcessorMemory::BlockSize; });
    for (int i = 0; i < _accumulated.noteEvents.size(); i++)
      _accumulated.noteEvents[i].position -= ProcessorMemory::BlockSize;
    _accumulatedInputSampleCount -= ProcessorMemory::BlockSize;
  }

  // so now we have accumulated N samples, push
  // them to host, left-zero-padding if missing,
  // and then shift the remainder of output buildup
  // this is where PDC comes into play!
  int hostOutputSample = 0;
  int samplesToPad = std::max(0, hostBlock.currentSampleCount - _accumulatedOutputSampleCount);
  int accumulatedSamplesToUse = hostBlock.currentSampleCount - samplesToPad;
  for (int sample = 0; sample < samplesToPad; sample++, hostOutputSample++)
    for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      hostBlock.audioOut[channel][hostOutputSample] = 0.0f;
  for (int sample = 0; sample < accumulatedSamplesToUse; sample++, hostOutputSample++)
    for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      hostBlock.audioOut[channel][hostOutputSample] = _accumulated.audioOut[channel][sample];
  for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
    _accumulated.audioOut[channel].erase(
      _accumulated.audioOut[channel].begin(),
      _accumulated.audioOut[channel].begin() + accumulatedSamplesToUse);
  _accumulatedOutputSampleCount -= accumulatedSamplesToUse;
}
