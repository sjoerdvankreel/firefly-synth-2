#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <chrono>

struct FBModuleProcState;

class FFOutputProcessor final
{
  std::chrono::steady_clock::time_point _cpuUpdated = {};

public:
  void Process(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOutputProcessor);
};