#pragma once

#include <playground_base/base/topo/FBListItem.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <string>
#include <vector>
#include <functional>

struct FBProcParamState;
struct FBProcStateAddrs;
struct FBScalarStateAddrs;

struct FBStaticParam
{
  int slotCount = {};
  int valueCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::vector<FBListItem> list = {};

  double DiscreteToNormalized(int index) const
  { return index / (valueCount - 1.0); }
  bool NormalizedToBool(double normalized) const
  { return NormalizedToDiscrete(normalized) != 0; }
  int NormalizedToDiscrete(double normalized) const
  { return std::min(valueCount - 1, (int)(normalized * valueCount)); }

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
  std::string NormalizedToText(bool io, double normalized) const;
  std::function<float* (int moduleSlot, int paramSlot, FBScalarStateAddrs& state)> scalarAddr;
  std::function<FBProcParamState* (int moduleSlot, int paramSlot, FBProcStateAddrs& state)> procAddr;
};