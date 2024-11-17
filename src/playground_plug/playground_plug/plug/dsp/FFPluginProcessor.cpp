#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <cassert>
#include <algorithm>

FFPluginProcessor::
FFPluginProcessor(int maxBlockSize, float sampleRate):
FBPluginProcessor(maxBlockSize, sampleRate)
{
  _processorBlock.sampleRate = sampleRate;
}

// run one round of fixed block size
void 
FFPluginProcessor::ProcessInternal()
{
  _processorBlock.waveShaperAudioInput[0].SetToZero();
  for (int osci = 0; osci < FF_OSCILLATOR_COUNT; osci++)
  {
    _processors.oscillator[osci].Process(osci, _processorBlock);
    _processorBlock.waveShaperAudioInput[0].InPlaceAdd(_processorBlock.oscillatorAudioOutput[osci]);
  }
  _processorBlock.waveShaperAudioInput[0].InPlaceMultiply(0.5f);
  _processors.waveShaper[0].Process(0, _processorBlock);
  _processorBlock.waveShaperAudioInput[1] = _processorBlock.waveShaperAudioOutput[0];
  _processors.waveShaper[1].Process(1, _processorBlock);
  _masterOutput = _processorBlock.waveShaperAudioOutput[1];
}