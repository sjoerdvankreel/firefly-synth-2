#pragma once

#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>
#include <cassert>

// handles fixed block sizes
// i don't know how to do this without crtp and no virtuals
template <class Derived, int BlockSize>
class FBPluginProcessor
{
  int const _maxRemaining;
  std::array<std::vector<float>, 2> _remainingOutput = {};

protected:
  float const _sampleRate;
  FBStereoBlock<BlockSize> _blockOutput = {};
  FBPluginProcessor(int maxBlockSize, float sampleRate);

public:
  void Process(
    FB_RAW_AUDIO_INPUT_BUFFER input, 
    FB_RAW_AUDIO_OUTPUT_BUFFER output, 
    int sampleCount);
};

template <class Derived, int BlockSize>
FBPluginProcessor<Derived, BlockSize>::
FBPluginProcessor(int maxBlockSize, float sampleRate):
_sampleRate(sampleRate),
_maxRemaining(std::max(maxBlockSize, BlockSize))
{
  _remainingOutput[0].resize(_maxRemaining);
  _remainingOutput[1].resize(_maxRemaining);
}

template <class Derived, int BlockSize> void
FBPluginProcessor<Derived, BlockSize>::Process(
  FB_RAW_AUDIO_INPUT_BUFFER input, 
  FB_RAW_AUDIO_OUTPUT_BUFFER output, 
  int sampleCount)
{
  // handle leftover from the previous round
  int samplesProcessed = 0;
  for (int s = 0; s < sampleCount && s < _remainingOutput[0].size(); s++)
  {
    for(int channel = 0; channel < 2; channel++)
      output[channel][samplesProcessed] = _remainingOutput[channel][s];
    samplesProcessed++;
  }

  // delete processed leftover from the remaining buffer
  for (int channel = 0; channel < 2; channel++)
    _remainingOutput[channel].erase(
      _remainingOutput[channel].begin(),
      _remainingOutput[channel].begin() + samplesProcessed);

  while (samplesProcessed < sampleCount)
  {
    static_cast<Derived*>(this)->ProcessBlock();

    // process in chunks of internal block size, may cause leftovers
    int blockSample = 0;
    for (; blockSample < BlockSize && samplesProcessed < sampleCount; blockSample++)
    {
      for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
        output[channel][samplesProcessed] = _blockOutput[channel][blockSample];
      samplesProcessed++;
    }

    // if we could not process all of the last internal block, stick it in the remaining buffer
    for (; blockSample < BlockSize; blockSample++)
    {
      for (int channel = 0; channel < 2; channel++)
        _remainingOutput[channel].push_back(_blockOutput[channel][blockSample]);
      samplesProcessed++;
      assert(_remainingOutput[0].size() <= _maxRemaining);
    }
  }
}