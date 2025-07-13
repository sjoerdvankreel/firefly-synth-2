#include <firefly_base/base/topo/static/FBParamsDependencies.hpp>

void
FBParamsDependency::WhenSlots(
  std::vector<FBTopoIndices> const& staticParamIndices_,
  FBParamsDependencyEvaluator const& evaluate_)
{
  evaluate = evaluate_;
  staticParamIndices = staticParamIndices_;
}

void
FBParamsDependency::WhenSimple(
  std::vector<int> const& staticParamIndices_,
  FBParamsDependencyEvaluator const& evaluate_)
{
  evaluate = evaluate_;
  staticParamIndices.clear();
  for (int i = 0; i < staticParamIndices_.size(); i++)
    staticParamIndices.push_back({ staticParamIndices_[i], -1 });
}