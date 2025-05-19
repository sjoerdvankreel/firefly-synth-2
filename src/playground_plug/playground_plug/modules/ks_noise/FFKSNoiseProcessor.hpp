#pragma once

#include <playground_plug/modules/ks_noise/FFKSNoiseTopo.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoisePhaseGenerator.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBMarsagliaPRNG.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <juce_dsp/juce_dsp.h>

#include <array>
#include <vector>

class FBAccParamState;
struct FBModuleProcState;

inline int constexpr FFKSNoiseWaveTableSize = 4096;
inline int constexpr MaxLen = 4096;

// https://forum.juce.com/t/fractional-delay-line/31946/5
class DelayLine {
public:
  DelayLine() = default;


  float process(float input)
  {
    // read from FIFO and interpolate
    auto frac = read - std::floorf(read);
    auto x0 = (int)std::floorf(read);
    auto x1 = (x0 + 1) % MaxLen;
    auto y0 = fifo[x0];
    auto y1 = fifo[x1];

    auto output = (1.0 - frac) * y0 + frac * y1;

    // write to FIFO 
    //fifo[write] = input; // TODO

    // increment counters
    write = (write + 1) % MaxLen;
    read = std::fmod(read + 1.0, float(MaxLen));

    return frac;
  }

  void setVal(int n, float val) { fifo[n] = val; }

  void setLen(float delaySamples) {
    auto newRead = float(write) + std::max(delaySamples, float(MaxLen));
    read = std::fmod(newRead, float(MaxLen));
  }

private:
  int write = 0;
  float read = 0.0;
  float fifo[MaxLen] = { static_cast<float> (0) };
};

class FFKSNoiseProcessor final:
public FFOsciProcessorBase
{
  int _seed = {};
  int _poles = {};
  FFKSNoiseType _type = {};

  int _graphPosition = 0;
  float _lastDraw = 0.0f;
  float _phaseTowardsX = 0.0f;
  
  FBMarsagliaPRNG _normalPrng = {};
  FBParkMillerPRNG _uniformPrng = {};
  FFKSNoisePhaseGenerator _phaseGen = {};

  int _colorFilterPosition = 0;
  DelayLine _dl = {};
  //FBSIMDArray<float, FFKSNoiseWaveTableSize> _waveTable = {};
  FBSIMDArray<float, FFKSNoiseMaxPoles> _colorFilterBuffer = {};

  float Draw();
  float Next(
    FBStaticModule const& topo,
    float sampleRate, float baseFreq,
    float colorNorm, float xNorm, float yNorm);

public:
  FFKSNoiseProcessor();
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFKSNoiseProcessor);
};