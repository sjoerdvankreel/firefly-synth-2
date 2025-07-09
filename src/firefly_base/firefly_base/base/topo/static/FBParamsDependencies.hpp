#pragma once

#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>
#include <vector>
#include <functional>

typedef std::function<bool(
  std::vector<int> const& runtimeParamValues)>
FBParamsDependencyEvaluator;

struct FBParamsDependency final
{
  std::vector<FBTopoIndices> staticParamIndices = {};
  FBParamsDependencyEvaluator evaluate = {};
  void When(
    std::vector<int> const& staticParamIndices,
    FBParamsDependencyEvaluator const& evaluate);
  void When(
    std::vector<FBTopoIndices> const& staticParamIndices,
    FBParamsDependencyEvaluator const& evaluate);
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