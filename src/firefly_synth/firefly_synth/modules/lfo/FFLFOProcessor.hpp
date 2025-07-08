#pragma once

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
  std::array<int, (int)FFLFOBlockCount> _steps = {};
  std::array<float, (int)FFLFOBlockCount> _phase = {};
  std::array<FFLFOOpType, (int)FFLFOBlockCount> _opType = {};
  std::array<FFLFOWaveMode, (int)FFLFOBlockCount> _waveMode = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOProcessor);

  template <bool Global>
  int Process(FBModuleProcState& state);
  template <bool Global>
  void BeginVoiceOrBlock(FBModuleProcState& state);
};