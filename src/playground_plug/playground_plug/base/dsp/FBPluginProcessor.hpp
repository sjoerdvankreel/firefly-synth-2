#pragma once

#include <playground_plug/base/shared/FBPluginBlocks.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>
#include <cassert>

// handles fixed block sizes
// i don't know how to do this without crtp and no virtuals!
template <int BlockSize, class Derived>
class FBPluginProcessor
{
  int const _maxRemaining;
  std::array<std::vector<float>, 2> _remainingOutputBuffer = {};
  
protected:
  float const _sampleRate;
  FBStereoBlock<BlockSize> _masterOutput = {};
  FBPluginProcessor(int maxBlockSize, float sampleRate);

public:
  void Process(
    FB_RAW_AUDIO_INPUT_BUFFER inputBuffer, 
    FB_RAW_AUDIO_OUTPUT_BUFFER outputBuffer, 
    int sampleCount);
};

template <int BlockSize, class Derived>
FBPluginProcessor<BlockSize, Derived>::
FBPluginProcessor(int maxBlockSize, float sampleRate):
_sampleRate(sampleRate),
_maxRemaining(std::max(maxBlockSize, BlockSize))
{
  _remainingOutputBuffer[0].resize(_maxRemaining);
  _remainingOutputBuffer[1].resize(_maxRemaining);
}

template <int BlockSize, class Derived> void 
FBPluginProcessor<BlockSize, Derived>::Process(
  FB_RAW_AUDIO_INPUT_BUFFER inputBuffer, 
  FB_RAW_AUDIO_OUTPUT_BUFFER outputBuffer, 
  int sampleCount)
{
  // handle leftover from the previous round
  int samplesProcessed = 0;
  for (int s = 0; s < sampleCount && s < _remainingOutputBuffer[0].size(); s++)
  {
    for(int channel = 0; channel < 2; channel++)
      outputBuffer[channel][samplesProcessed] = _remainingOutputBuffer[channel][s];
    samplesProcessed++;
  }

  // delete processed leftover from the remaining buffer
  for (int channel = 0; channel < 2; channel++)
    _remainingOutputBuffer[channel].erase(
      _remainingOutputBuffer[channel].begin(), 
      _remainingOutputBuffer[channel].begin() + samplesProcessed);

  while (samplesProcessed < sampleCount)
  {
    static_cast<Derived*>(this)->ProcessInternal();

    // process in chunks of internal block size, may cause leftovers
    int blockSample = 0;
    for (; blockSample < BlockSize && samplesProcessed < sampleCount; blockSample++)
    {
      for (int channel = 0; channel < 2; channel++)
        outputBuffer[channel][samplesProcessed] = _masterOutput[channel][blockSample];
      samplesProcessed++;
    }

    // if we could not process all of the last internal block, stick it in the remaining buffer
    for (; blockSample < BlockSize; blockSample++)
    {
      for (int channel = 0; channel < 2; channel++)
        _remainingOutputBuffer[channel].push_back(_masterOutput[channel][blockSample]);
      samplesProcessed++;
      assert(_remainingOutputBuffer[0].size() <= _maxRemaining);
    }
  }
}