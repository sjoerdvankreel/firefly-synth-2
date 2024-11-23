#if 0

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
FFOsciProcessor::Process(FBProcessorContext const& context, FFProcessorMemory& memory)
{
  auto const& plugState = memory.plugParam.osci[context.moduleSlot];
  bool on = FBNormalizedToBool(plugState[FFOsciPlugParamOn]);
  if (on)
    ProcessType(context, memory);
  else
    memory.osciOut[context.moduleSlot].SetToZero();
}

void 
FFOsciProcessor::ProcessType(FBProcessorContext const& context, FFProcessorMemory& memory)
{
  auto const& plugState = memory.plugParam.osci[context.moduleSlot];
  int type = FBNormalizedToDiscrete(FFOsciTypeCount, plugState[FFOsciPlugParamType]);
  switch (type)
  {
  case FFOsciTypeSaw: 
    ProcessType(context, memory, CalcSaw);
    break;
  case FFOsciTypeSine: 
    ProcessType(context, memory, CalcSine);
    break;
  default: 
    assert(false); 
    break;
  }
}

template <class Calc> void
FFOsciProcessor::ProcessType(FBProcessorContext const& context, FFProcessorMemory& memory, Calc calc)
{
  auto const& autoState = memory.denseAutoParam.osci[context.moduleSlot];
  auto const& gain = autoState[FFOsciAutoParamGain];

  for (int s = 0; s < FF_BLOCK_SIZE; s++)
  {
    // todo pitch
    _phase += 440.0f / context.sampleRate;
    _phase -= std::floor(_phase);
    float sample = calc(_phase) * gain[s];
    for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      memory.osciOut[context.moduleSlot][channel][s] = sample;
  }
}

#endif