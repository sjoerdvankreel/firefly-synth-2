#pragma once

#include <playground_base/base/shared/FBUtility.hpp>

struct FBModuleProcExchangeState
{
  bool active = {};
  int lengthSamples = {};
  int positionSamples = {};
  FB_COPY_MOVE_DEFCTOR(FBModuleProcExchangeState);

  bool ShouldGraph() const { return active && positionSamples < lengthSamples; }
  float PositionNormalized() const { return positionSamples / static_cast<float>(lengthSamples); }
};