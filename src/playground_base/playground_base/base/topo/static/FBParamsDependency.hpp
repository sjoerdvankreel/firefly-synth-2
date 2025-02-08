#pragma once

#include <vector>
#include <functional>

typedef std::function<bool(
  std::vector<int> const& runtimeParamIndices)>
FBParamsDependencyEvaluator;

struct FBParamsDependency final
{
  std::vector<int> staticParamIndices = {};
  FBParamsDependencyEvaluator evaluate = {};
  void When(
    std::vector<int> const& staticParamIndices, 
    FBParamsDependencyEvaluator const& evaluate);
};