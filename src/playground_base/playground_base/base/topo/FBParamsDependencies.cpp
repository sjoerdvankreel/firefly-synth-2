#include <playground_base/base/topo/FBParamsDependencies.hpp>

void
FBParamsDependency::When(
  std::vector<int> const& params_,
  FBParamsDependencyEvaluator const& evaluate_)
{
  params = params_;
  evaluate = evaluate_;
}