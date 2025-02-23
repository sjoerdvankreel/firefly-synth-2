#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBModuleProcExchangeState final
{
  bool active = {};
  float lastOutput = {};
  int lengthSamples = {};
  int positionSamples = {};
  FB_COPY_MOVE_DEFCTOR(FBModuleProcExchangeState);

  bool ShouldGraph() const { return active && positionSamples < lengthSamples; }
  float PositionNormalized() const { return positionSamples / (float)lengthSamples; }
};