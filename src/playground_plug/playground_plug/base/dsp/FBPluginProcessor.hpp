#pragma once

#include <playground_plug/base/dsp/FBHostBlock.hpp>
#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>
#include <cassert>

// handles fixed block sizes
// i don't know how to do this without crtp and no virtuals
template <class Derived, class PluginBlock, int PluginBlockSize>
class FBPluginProcessor
{
  int const _maxRemaining;
  std::array<std::vector<float>, 2> _remainingOut = {};

protected:
  float const _sampleRate;
  PluginBlock _pluginBlock = {};
  FBPluginProcessor(int maxHostBlockSize, float sampleRate);

public:
  void ProcessHostBlock(FBHostBlock& hostBlock);
};

template <class Derived, class PluginBlock, int PluginBlockSize>
FBPluginProcessor<Derived, PluginBlock, PluginBlockSize>::
FBPluginProcessor(int maxHostBlockSize, float sampleRate):
_sampleRate(sampleRate),
_maxRemaining(std::max(maxHostBlockSize, PluginBlockSize))
{
  _remainingOut[0].resize(_maxRemaining);
  _remainingOut[1].resize(_maxRemaining);
}

template <class Derived, class PluginBlock, int PluginBlockSize> void
FBPluginProcessor<Derived, PluginBlock, PluginBlockSize>::ProcessHostBlock(FBHostBlock& hostBlock)
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

  while (samplesProcessed < hostBlock.sampleCount)
  {
    static_cast<Derived*>(this)->ProcessPluginBlock();

    // process in chunks of internal block size, may cause leftovers
    int blockSample = 0;
    for (; blockSample < PluginBlockSize && samplesProcessed < hostBlock.sampleCount; blockSample++)
    {
      for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
        hostBlock.audioOut[channel][samplesProcessed] = _pluginBlock.masterOut[channel][blockSample];
      samplesProcessed++;
    }

    // if we could not process all of the last internal block, stick it in the remaining buffer
    for (; blockSample < PluginBlockSize; blockSample++)
    {
      for (int channel = 0; channel < 2; channel++)
        _remainingOut[channel].push_back(_pluginBlock.masterOut[channel][blockSample]);
      samplesProcessed++;
      assert(_remainingOut[0].size() <= _maxRemaining);
    }
  }
}