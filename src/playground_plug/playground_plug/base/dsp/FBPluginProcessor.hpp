#pragma once

#include <playground_plug/base/dsp/FBHostBlock.hpp>
#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>
#include <cassert>

// handles fixed block sizes
// i don't know how to do this without crtp and no virtuals
template <class Derived, class ProcessorMemory>
class FBPluginProcessor
{
  float const _sampleRate;
  int const _maxRemaining;
  std::array<std::vector<float>, 2> _remainingOut = {};

public:
  void ProcessHostBlock(FBHostBlock& hostBlock);

protected:
  ProcessorMemory _memory = {};
  FBRuntimeTopo<ProcessorMemory> const* const _topo;
  FBPluginProcessor(FBRuntimeTopo<ProcessorMemory> const* topo, int maxHostBlockSize, float sampleRate);
};

template <class Derived, class ProcessorMemory>
FBPluginProcessor<Derived, ProcessorMemory>::
FBPluginProcessor(FBRuntimeTopo<ProcessorMemory> const* topo, int maxHostBlockSize, float sampleRate):
_topo(topo),
_sampleRate(sampleRate),
_maxRemaining(std::max(maxHostBlockSize, ProcessorMemory::BlockSize))
{
  _remainingOut[0].resize(_maxRemaining);
  _remainingOut[1].resize(_maxRemaining);
}

template <class Derived, class ProcessorMemory> void
FBPluginProcessor<Derived, ProcessorMemory>::ProcessHostBlock(FBHostBlock& hostBlock)
{
  // handle leftover from the previous round
  int samplesProcessed = 0;
  for (int s = 0; s < hostBlock.sampleCount && s < _remainingOut[0].size(); s++)
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
  while (samplesProcessed < hostBlock.sampleCount)
  {
    static_cast<Derived*>(this)->ProcessPluginBlock(context);

    // process in chunks of internal block size, may cause leftovers
    int blockSample = 0;
    for (; blockSample < ProcessorMemory::BlockSize && samplesProcessed < hostBlock.sampleCount; blockSample++)
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
      assert(_remainingOut[0].size() <= _maxRemaining);
    }
  }
}