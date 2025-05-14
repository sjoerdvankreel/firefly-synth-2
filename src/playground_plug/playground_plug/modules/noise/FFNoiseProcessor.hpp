#pragma once

#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFNoiseProcessor final
{
  float _key = {};
  bool _on = {};
  int _uniCount = {};
  int _seed = {};
  int _q = {};

  int _position = 0;
  float _baseFreq = 0.0f; // todo remove
  FBParkMillerPRNG _prng = {};
  FBSIMDArray<float, FFNoiseMaxQ> _w = {};
  FBSIMDArray2<float, FFOsciBaseUniMaxCount, FFNoiseMaxQ> _x = {};
  FBSIMDArray2<float, FBFixedBlockSamples, FFOsciBaseUniMaxCount> _uniOutput = {};

public:
  FFNoiseProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFNoiseProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};