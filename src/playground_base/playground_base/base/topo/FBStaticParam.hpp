#pragma once

#include <playground_base/base/topo/FBListItem.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <string>
#include <vector>
#include <functional>

struct FBProcParamState;

typedef std::function<float* (
  int moduleSlot, int paramSlot, void* state)>
FBFloatAddrSelector;
typedef std::function<FBProcParamState* (
  int moduleSlot, int paramSlot, void* state)>
FBProcParamAddrSelector;

struct FBStaticParam final
{
  int slotCount = {};
  int valueCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::vector<FBListItem> list = {};

  FBFloatAddrSelector scalarAddr = {};
  FBFloatAddrSelector procBlockAddr = {};
  FBProcParamAddrSelector procAccAddr = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);
  std::string NormalizedToText(bool io, double normalized) const;
  
  double DiscreteToNormalized(int index) const
  { return index / (valueCount - 1.0); }
  bool NormalizedToBool(double normalized) const
  { return NormalizedToDiscrete(normalized) != 0; }
  int NormalizedToDiscrete(double normalized) const
  { return std::min(valueCount - 1, (int)(normalized * valueCount)); }
};