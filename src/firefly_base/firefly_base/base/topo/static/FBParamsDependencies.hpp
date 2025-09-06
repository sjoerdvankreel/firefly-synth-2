#pragma once

#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>
#include <vector>
#include <functional>

typedef std::function<bool(
  std::vector<int> const& runtimeParamValues)>
FBSimpleParamsDependencyEvaluator;

typedef std::function<bool(
  std::vector<int> const& runtimeParamSlots,
  std::vector<int> const& runtimeParamValues)>
  FBSlotParamsDependencyEvaluator;

struct FBParamsDependency final
{
  std::vector<FBTopoIndices> staticParamIndices = {};
  FBSlotParamsDependencyEvaluator evaluate = {};
  void WhenSimple(
    std::vector<int> const& staticParamIndices,
    FBSimpleParamsDependencyEvaluator const& evaluate);
  void WhenSlots(
    std::vector<FBTopoIndices> const& staticParamIndices,
    FBSlotParamsDependencyEvaluator const& evaluate);
};

struct FBAnyParamsDependencies final
{
  FBParamsDependency gui = {};
  FBParamsDependency audio = {};
};

struct FBParamsDependencies final
{
  FBAnyParamsDependencies visible = {};
  FBAnyParamsDependencies enabled = {};
};