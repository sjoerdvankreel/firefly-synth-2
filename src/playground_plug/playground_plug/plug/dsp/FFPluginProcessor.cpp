#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <cassert>
#include <algorithm>

FFPluginProcessor::
FFPluginProcessor(int maxBlockSize, float sampleRate):
_sampleRate(sampleRate),
_maxRemaining(std::max(maxBlockSize, FF_BLOCK_SIZE))
{
  _processorBlock.sampleRate = sampleRate;
  _remainingOutputBuffer[0].resize(_maxRemaining);
  _remainingOutputBuffer[1].resize(_maxRemaining);
}

void 
FFPluginProcessor::Process(
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
    ProcessInternal();

    // process in chunks of internal block size, may cause leftovers
    int blockSample = 0;
    for (; blockSample < FF_BLOCK_SIZE && samplesProcessed < sampleCount; blockSample++)
    {
      for (int channel = 0; channel < 2; channel++)
        outputBuffer[channel][samplesProcessed] = _processorBlock.masterOutput[channel][blockSample];
      samplesProcessed++;
    }

    // if we could not process all of the last internal block, stick it in the remaining buffer
    for (; blockSample < FF_BLOCK_SIZE; blockSample++)
    {
      for (int channel = 0; channel < 2; channel++)
        _remainingOutputBuffer[channel].push_back(_processorBlock.masterOutput[channel][blockSample]);
      samplesProcessed++;
      assert(_remainingOutputBuffer[0].size() <= _maxRemaining);
    }
  }
}

// run one round of fixed block size
void 
FFPluginProcessor::ProcessInternal()
{
  for (int osci = 0; osci < FF_OSCILLATOR_COUNT; osci++)
    _processors.oscillator[osci].Process(osci, _processorBlock);
  for (int channel = 0; channel < 2; channel++)
  {
    _processorBlock.waveShaperAudioInput[0][channel].fill(0.0f);
    for (int osci = 0; osci < FF_OSCILLATOR_COUNT; osci++)
      for (int s = 0; s < FF_BLOCK_SIZE; s++)
        _processorBlock.waveShaperAudioInput[0][channel][s] += _processorBlock.oscillatorAudioOutput[osci][channel][s];
  }
}