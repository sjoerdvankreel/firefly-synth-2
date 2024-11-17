#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <cassert>
#include <algorithm>

FFPluginProcessor::
FFPluginProcessor(int maxHostBlockSize, float sampleRate):
FBPluginProcessor(maxHostBlockSize, sampleRate)
{
  _processorBlock.sampleRate = sampleRate;
}

// run one round of fixed block size
void 
FFPluginProcessor::ProcessPluginBlock()
{
  _processorBlock.shaperInput[0].SetToZero();
  for (int osci = 0; osci < FF_OSCI_COUNT; osci++)
  {
    _processors.osci[osci].Process(osci, _processorBlock);
    _processorBlock.shaperInput[0].InPlaceAdd(_processorBlock.osciOutput[osci]);
  }
  _processorBlock.shaperInput[0].InPlaceMultiply(0.5f);
  _processors.shaper[0].Process(0, _processorBlock);
  _processorBlock.shaperOutput[0].CopyTo(_processorBlock.shaperInput[1]);
  _processors.shaper[1].Process(1, _processorBlock);
  _processorBlock.shaperOutput[1].CopyTo(_pluginBlockOutput);
}