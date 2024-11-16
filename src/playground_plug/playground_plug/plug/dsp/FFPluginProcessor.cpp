#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <cassert>

FFPluginProcessor::
FFPluginProcessor(std::size_t maxBlockSize, float sampleRate):
_maxBlockSize(maxBlockSize),
_sampleRate(sampleRate)
{
  _remainingOutputBuffer[0].resize(maxBlockSize);
  _remainingOutputBuffer[1].resize(maxBlockSize);
}

void 
FFPluginProcessor::Process(
  FB_RAW_AUDIO_INPUT_BUFFER inputBuffer, 
  FB_RAW_AUDIO_OUTPUT_BUFFER outputBuffer, 
  std::size_t sampleCount)
{
  // handle leftover from the previous round
  std::size_t samplesProcessed = 0;
  for (std::size_t sample = 0; sample < sampleCount && sample < _remainingOutputBuffer[0].size(); sample++)
  {
    for(std::size_t channel = 0; channel < 2; channel++)
      outputBuffer[channel][sample] = _remainingOutputBuffer[channel][sample];
    samplesProcessed++;
  }

  // delete processed leftover from the remaining buffer
  for (std::size_t channel = 0; channel < 2; channel++)
    _remainingOutputBuffer[channel].erase(
      _remainingOutputBuffer[channel].begin(), 
      _remainingOutputBuffer[channel].begin() + samplesProcessed);

  while (samplesProcessed < sampleCount)
  {
    _oscillatorProcessor.Process(_sampleRate, _oscillatorBlock);

    // process in chunks of internal block size, may cause leftovers
    std::size_t blockSample = 0;
    for (; blockSample < FF_BLOCK_SIZE && samplesProcessed < sampleCount; blockSample++)
    {
      for (std::size_t channel = 0; channel < 2; channel++)
        outputBuffer[channel][samplesProcessed + blockSample] = _oscillatorBlock[channel][blockSample];
      samplesProcessed++;
    }

    // if we could not process all of the last internal block, stick it in the remaining buffer
    for (; blockSample < FF_BLOCK_SIZE; blockSample++)
    {
      for (std::size_t channel = 0; channel < 2; channel++)
        _remainingOutputBuffer[channel].push_back(_oscillatorBlock[channel][blockSample]);
      samplesProcessed++;
      assert(_remainingOutputBuffer[0].size() < _maxBlockSize);
    }
  }
}