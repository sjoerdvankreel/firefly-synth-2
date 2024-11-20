#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>
#include <cassert>
#include <algorithm>

FFPluginProcessor::
FFPluginProcessor(FFRuntimeTopo const* topo, int maxSampleCount, float sampleRate):
FBPluginProcessor(topo, maxSampleCount, sampleRate)
{
}

// run one round of fixed block size
void 
FFPluginProcessor::ProcessPluginBlock(FBProcessorContext const& context)
{
  auto moduleContext = context;
  _memory.shaperIn[0].SetToZero();
  for (int osci = 0; osci < FF_OSCI_COUNT; osci++)
  {
    moduleContext.moduleSlot = osci;
    _processors.osci[osci].Process(moduleContext, _memory);
    _memory.shaperIn[0].InPlaceAdd(_memory.osciOut[osci]);
  }
  _memory.shaperIn[0].InPlaceMultiply(0.5f);

  moduleContext.moduleSlot = 0;
  _processors.shaper[0].Process(moduleContext, _memory);
  _memory.shaperOut[0].CopyTo(_memory.shaperIn[1]);

  moduleContext.moduleSlot = 1;
  _processors.shaper[1].Process(moduleContext, _memory);
  _memory.shaperOut[1].CopyTo(_memory.masterOut);
}