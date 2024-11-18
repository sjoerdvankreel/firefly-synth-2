#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <cmath>
#include <numbers>
#include <cassert>

static float
CalcSaw(float phase)
{
  return (phase * 2.0f) - 1.0f; // TODO blep
}

static float
CalcSine(float phase)
{
  float radians = phase * 2.0f * std::numbers::pi_v<float>;
  return std::sin(radians);
}

void
FFOsciProcessor::Process(int moduleSlot, FFProcessorMemory& memory)
{
  auto const& plugState = memory.paramMemory.osciPlug[moduleSlot];
  bool on = FBNormalizedToBool(plugState[FFOsciPlugParamOn]);
  if (on)
    ProcessType(moduleSlot, memory);
  else
    memory.osciOut[moduleSlot].SetToZero();
}

void 
FFOsciProcessor::ProcessType(int moduleSlot, FFProcessorMemory& memory)
{
  auto const& plugState = memory.paramMemory.osciPlug[moduleSlot];
  int type = FBNormalizedToDiscrete(FFOsciTypeCount, plugState[FFOsciPlugParamType]);
  switch (type)
  {
  case FFOsciTypeSaw: 
    ProcessType(moduleSlot, memory, CalcSaw);
    break;
  case FFOsciTypeSine: 
    ProcessType(moduleSlot, memory, CalcSine);
    break;
  default: 
    assert(false); 
    break;
  }
}

template <class Calc> void
FFOsciProcessor::ProcessType(int moduleSlot, FFProcessorMemory& memory, Calc calc)
{
  for (int s = 0; s < FF_BLOCK_SIZE; s++)
  {
    // todo pitch
    _phase += 440.0f / memory.sampleRate;
    _phase -= std::floor(_phase);
    float sample = calc(_phase);
    for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      memory.osciOut[moduleSlot][channel][s] = sample;
  }
}