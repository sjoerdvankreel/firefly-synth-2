#pragma once

#include <vector>
#include <functional>

typedef std::function<bool(std::vector<int> const& vs)>
FBParamsDependencyEvaluator;

struct FBParamsDependency
{
  std::vector<int> params = {};
  FBParamsDependencyEvaluator evaluate = {};
  void When(std::vector<int> const& params, FBParamsDependencyEvaluator const& evaluate);
};

struct FBParamsDependencies
{
  FBParamsDependency enabled = {};
  FBParamsDependency visible = {};
};