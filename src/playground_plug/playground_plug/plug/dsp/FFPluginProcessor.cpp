#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <cassert>
#include <algorithm>

FFPluginProcessor::
FFPluginProcessor(int maxBlockSize, float sampleRate):
FBPluginProcessor(maxBlockSize, sampleRate)
{
  _block.sampleRate = sampleRate;
}

// run one round of fixed block size
void 
FFPluginProcessor::ProcessBlock()
{
  _block.shaperInput[0].SetToZero();
  for (int osci = 0; osci < FF_OSCI_COUNT; osci++)
  {
    _processors.osci[osci].Process(osci, _block);
    _block.shaperInput[0].InPlaceAdd(_block.osciOutput[osci]);
  }
  _block.shaperInput[0].InPlaceMultiply(0.5f);
  _processors.shaper[0].Process(0, _block);
  _block.shaperInput[1] = _block.shaperInput[0];
  _processors.shaper[1].Process(1, _block);
  _blockOutput = _block.shaperInput[1];
}