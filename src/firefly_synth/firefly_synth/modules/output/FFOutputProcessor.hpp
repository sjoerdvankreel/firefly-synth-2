#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <chrono>

struct FBRuntimeTopo;
struct FBModuleProcState;
struct FBPlugOutputBlock;

class FFOutputProcessor final
{
  float _maxCpu = {};
  float _maxGain = {};
  float _maxVoices = {};

  int _cpuParamIndex = -1;
  int _gainParamIndex = -1;
  int _voicesParamIndex = -1;

  std::chrono::steady_clock::time_point _updated = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOutputProcessor);
  void Process(FBModuleProcState& state, FBPlugOutputBlock const& output);
};