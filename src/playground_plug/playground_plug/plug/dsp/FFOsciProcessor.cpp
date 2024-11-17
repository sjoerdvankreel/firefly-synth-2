#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <cmath>
#include <numbers>
#include <cassert>

static float
PhaseToOutSaw(float phase)
{
  return (phase * 2.0f) - 1.0f; // TODO blep
}

static float
PhaseToOutSine(float phase)
{
  return std::sin(phase * 2.0f * std::numbers::pi_v<float>);
}

void
FFOsciProcessor::ProcessOff(int moduleSlot, FFPluginBlock& block)
{
  block.osciOut[moduleSlot].SetToZero();
}

template <class PhaseToOut> void 
FFOsciProcessor::ProcessPhase(int moduleSlot, FFPluginBlock& block, PhaseToOut phaseToOut)
{
  for (int s = 0; s < FF_BLOCK_SIZE; s++)
  {
    _phase += 440.0f / block.sampleRate;
    _phase -= std::floor(_phase);
    float sample = phaseToOut(_phase);
    for (int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      block.osciOut[moduleSlot][channel][s] = sample;
  }
}

void
FFOsciProcessor::Process(int moduleSlot, FFPluginBlock& block)
{
  auto const& myPlugState = block.paramMemory.osciPlug[moduleSlot];
  if (FBNormalizedToStep(1, myPlugState[FFOsciPlugParamOn]) == 0)
    ProcessOff(moduleSlot, block);
  else 
    switch (FBNormalizedToStep(FFOsciTypeCount - 1, myPlugState[FFOsciPlugParamType]))
    {
    case FFOsciTypeSaw:
      ProcessPhase(moduleSlot, block, PhaseToOutSaw);
      break;
    case FFOsciTypeSine:
      ProcessPhase(moduleSlot, block, PhaseToOutSine);
      break;
    default:
      assert(false);
      break;
    }
}