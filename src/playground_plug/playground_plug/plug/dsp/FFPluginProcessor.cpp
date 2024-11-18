#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <cassert>
#include <algorithm>

FFPluginProcessor::
FFPluginProcessor(FFRuntimeTopo const* topo, int maxHostBlockSize, float sampleRate):
FBPluginProcessor(topo, maxHostBlockSize, sampleRate)
{
}

// run one round of fixed block size
void 
FFPluginProcessor::ProcessPluginBlock()
{
  _memory.shaperIn[0].SetToZero();
  for (int osci = 0; osci < FF_OSCI_COUNT; osci++)
  {
    _processors.osci[osci].Process(osci, _memory);
    _memory.shaperIn[0].InPlaceAdd(_memory.osciOut[osci]);
  }
  _memory.shaperIn[0].InPlaceMultiply(0.5f);
  _processors.shaper[0].Process(0, _memory);
  _memory.shaperOut[0].CopyTo(_memory.shaperIn[1]);
  _processors.shaper[1].Process(1, _memory);
  _memory.shaperOut[1].CopyTo(_memory.masterOut);
}