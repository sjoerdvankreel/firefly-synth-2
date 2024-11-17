#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <cassert>
#include <algorithm>

FFPluginProcessor::
FFPluginProcessor(int maxHostBlockSize, float sampleRate):
FBPluginProcessor(maxHostBlockSize, sampleRate)
{
  _pluginBlock.sampleRate = sampleRate;
}

// run one round of fixed block size
void 
FFPluginProcessor::ProcessPluginBlock()
{
  _pluginBlock.shaperIn[0].SetToZero();
  for (int osci = 0; osci < FF_OSCI_COUNT; osci++)
  {
    _processors.osci[osci].Process(osci, _pluginBlock);
    _pluginBlock.shaperIn[0].InPlaceAdd(_pluginBlock.osciOut[osci]);
  }
  _pluginBlock.shaperIn[0].InPlaceMultiply(0.5f);
  _processors.shaper[0].Process(0, _pluginBlock);
  _pluginBlock.shaperOut[0].CopyTo(_pluginBlock.shaperIn[1]);
  _processors.shaper[1].Process(1, _pluginBlock);
  _pluginBlock.shaperOut[1].CopyTo(_pluginBlock.masterOut);
}