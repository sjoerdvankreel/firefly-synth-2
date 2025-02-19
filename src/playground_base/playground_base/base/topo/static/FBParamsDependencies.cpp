#include <playground_base/base/topo/static/FBParamsDependencies.hpp>

void
FBParamsDependency::When(
  std::vector<int> const& staticParamIndices_,
  FBParamsDependencyEvaluator const& evaluate_)
{
  evaluate = evaluate_;
  staticParamIndices = staticParamIndices_;
}