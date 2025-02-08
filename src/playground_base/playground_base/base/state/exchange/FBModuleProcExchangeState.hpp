#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBModuleProcExchangeState final
{
  bool active = {};
  int lengthSamples = {};
  int positionSamples = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBModuleProcExchangeState);
};