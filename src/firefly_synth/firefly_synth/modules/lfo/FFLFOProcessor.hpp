#pragma once

#include <firefly_synth/dsp/shared/FFPhaseGenerator.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;

class FFLFOProcessor final
{
  int _seed = {};
  bool _sync = {};
  FFLFOType _type = {};
  FFLFOSkewMode _skewXMode = {};
  FFLFOSkewMode _skewYMode = {};
  std::array<int, FFLFOBlockCount> _steps = {};
  std::array<FFLFOOpType, FFLFOBlockCount> _opType = {};
  std::array<FFLFOWaveMode, FFLFOBlockCount> _waveMode = {};

  std::array<FFTimeVectorPhaseGenerator, FFLFOBlockCount> _phaseGens = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOProcessor);

  template <bool Global>
  int Process(FBModuleProcState& state);
  template <bool Global>
  void BeginVoiceOrBlock(FBModuleProcState& state);
};